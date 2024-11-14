#include "utils/guild_audio_manager.h"
#include "utils/audio.h"
#include "utils/track_library.h"
#include <chrono>
#include <dpp/snowflake.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

GuildAudioManager::GuildAudioManager(std::shared_ptr<Audio> _audio, std::shared_ptr<TrackLibrary> _track_library)
    : audio(_audio), track_library(_track_library), stop_thread(false) {
    playback_thread = std::thread(&GuildAudioManager::playback_loop, this);
}

GuildAudioManager::~GuildAudioManager() {
    {
        std::lock_guard<std::mutex> lock(cv_mutex);
        stop_thread = true;
    }
    cv.notify_all();
    if (playback_thread.joinable()) {
        playback_thread.join();
    }
}

std::string GuildAudioManager::queue_track(dpp::snowflake guild_id, const std::string& track_name) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] Failed to get or create queue for guild " << guild_id << std::endl;
        return "ERROR";
    }

    std::shared_ptr<Track> track = track_library->get_track(track_name);
    if (!track) {
        std::cerr << "[GuildAudioManager] Track '" << track_name << "' not found in library." << std::endl;
        return "ERROR";
    }

    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        guild_queue->tracks.push(track);
    }
    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued track: " << track->get_name() << " in guild: " << guild_id << std::endl; 
    return track->get_name();
}

void GuildAudioManager::queue_all(dpp::snowflake guild_id) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] Failed to get or create queue for guild " << guild_id << std::endl;
        return;
    }

    std::vector<std::string> all_track_names = track_library->get_all_track_names();
    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        for (const auto& track_name : all_track_names) {
            std::shared_ptr<Track> track = track_library->get_track(track_name);
            if (track) {
                guild_queue->tracks.push(track);
            } else {
                std::cerr << "[GuildAudioManager] Track '" << track_name << "' not found in library." << std::endl;
            }
        }
    }

    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued all tracks in guild: " << guild_id << std::endl;
}

void GuildAudioManager::skip_track(dpp::snowflake guild_id) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        if (guild_queue->is_playing) {
            guild_queue->skip = true;
            std::cout << "[GuildAudioManager] Skipping current track for guild " << guild_id << std::endl;
        }
    }

    cv.notify_all();
}

void GuildAudioManager::pause_track(dpp::snowflake guild_id) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        if (guild_queue->is_playing && !guild_queue->paused) {
            guild_queue->paused = true;
            std::cout << "[GuildAudioManager] Paused track for guild " << guild_id << std::endl;
        }
    }
}

void GuildAudioManager::resume_track(dpp::snowflake guild_id) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        if (guild_queue->is_playing && guild_queue->paused) {
            guild_queue->paused = false;
            std::cout << "[GuildAudioManager] Resumed track for guild " << guild_id << std::endl;
        }
    }
    cv.notify_all();
}

void GuildAudioManager::clear_queue(dpp::snowflake guild_id) {
    std::shared_ptr<GuildQueue> guild_queue;
    {
        std::lock_guard<std::mutex> lock(manager_mutex);
        auto it = guild_queues.find(guild_id);
        if (it != guild_queues.end()) {
            guild_queue = it->second;
            {
                std::lock_guard<std::mutex> queue_lock(guild_queue->queue_mutex);
                guild_queue->stop = true;
            }

            guild_queues.erase(it);
            std::cout << "[GuildAudioManager] Removed queue for guild " << guild_id << std::endl;
        }
    }

    cv.notify_all();

}

void GuildAudioManager::playback_loop() {
    while (true) {
        std::unique_lock<std::mutex> lock(cv_mutex);
        cv.wait(lock, [this]() {
            return stop_thread || !guild_queues.empty();
        });

        if (stop_thread) {
            break;
        }

        {
            std::lock_guard<std::mutex> manager_lock(manager_mutex);
            for (auto it = guild_queues.begin(); it != guild_queues.end(); ) {
                auto guild_id = it->first;
                std::shared_ptr<GuildQueue> guild_queue = it->second;

                {
                    std::lock_guard<std::mutex> queue_lock(guild_queue->queue_mutex);
                    if (guild_queue->stop) {
                        std::cout << "[GuildAudioManager] Exit requested for guild " << guild_id << std::endl;
                        it = guild_queues.erase(it);
                        continue;
                    }
                }

                {
                    std::lock_guard<std::mutex> queue_lock(guild_queue->queue_mutex);

                    if (!guild_queue->is_playing && !guild_queue->tracks.empty()) {
                        if (!audio->voice_ready(guild_id)) {
                            it++;
                            continue;
                        }
                        std::shared_ptr<Track> track = guild_queue->tracks.front();
                        guild_queue->current_track = track;
                        guild_queue->tracks.pop();
                        guild_queue->is_playing = true;
                        guild_queue->skip = false;
                        track->load();
                        std::cout << "[GuildAudioManager] playing track '" << guild_queue->current_track->get_name() << "' for guild " << guild_id << std::endl;
                        std::thread([this, guild_queue, track, guild_id]() {
                            audio->send_audio_to_voice(guild_id, track);
                            size_t length_ms = track->get_length();
                            size_t elapsed_ms = 0;
                            
                            while (elapsed_ms < length_ms) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                {
                                    std::unique_lock<std::mutex> lock(guild_queue->queue_mutex);
                                    if (guild_queue->paused) {
                                        cv.wait(lock, [&guild_queue]() { return !guild_queue->paused || guild_queue->stop || guild_queue->skip; });
                                    }
                                    if (guild_queue->skip || guild_queue->stop) {
                                        std::cout << "[GuildAudioManager] Track skipped or stop requested: " << track->get_name() << std::endl;
                                        guild_queue->is_playing = false;
                                        cv.notify_all();
                                        track->unload();
                                        return;
                                    }
                                }
                                elapsed_ms += 100;
                            }
                            if (guild_queue) {
                                std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
                                if (!guild_queue->stop) {
                                    guild_queue->is_playing = false;
                                }
                            }

                            std::cout << "[GuildAudioManager] Finished playing track " << track->get_name() << std::endl;
                            track->unload();
                            cv.notify_all();
                        }).detach();
                    }
                }

                ++it;            
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::shared_ptr<GuildQueue> GuildAudioManager::get_queue(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(manager_mutex);
    auto it = guild_queues.find(guild_id);
    if (it == guild_queues.end()) {
        auto guild_queue = std::make_shared<GuildQueue>();
        guild_queues[guild_id] = guild_queue;
        return guild_queue;
    }
    return it->second;
}

std::vector<std::string> GuildAudioManager::get_queued_tracks(dpp::snowflake guild_id) {
    auto queue = get_queue(guild_id);
    std::vector<std::string> tracks;
    if (!queue) {
        std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
        return tracks;
    }

    {
        std::lock_guard<std::mutex> lock(queue->queue_mutex);

        if (queue->is_playing) {
            tracks.push_back(queue->current_track->toString());
        }

        std::queue<std::shared_ptr<Track>> temp_queue = queue->tracks;
        while (!temp_queue.empty()) {
            if (temp_queue.front()) {
                tracks.push_back(temp_queue.front()->toString());
            } else {
                std::cerr << "[GuildAudioManager] Encountered a null track in the queue for guild " << guild_id << std::endl;
                tracks.push_back("Unknown Track");
            }
            temp_queue.pop();
        } 
    }

    return tracks;

}
