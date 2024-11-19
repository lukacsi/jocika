#include "utils/guild_audio_manager.h"
#include "utils/audio.h"
#include "utils/track_library.h"
#include <chrono>
#include <dpp/snowflake.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

GuildAudioManager::GuildAudioManager(std::shared_ptr<Audio> _audio, std::shared_ptr<TrackLibrary> _track_library)
    : audio(_audio), track_library(_track_library), stop_thread(false) {
    playback_thread = std::thread(&GuildAudioManager::playback_loop, this);
    std::random_device rd;
    g = std::mt19937(rd());
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

std::vector<std::shared_ptr<Track>> GuildAudioManager::queue_track(dpp::snowflake guild_id, const std::string& track_name) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] Failed to get or create queue for guild " << guild_id << std::endl;
        return {};
    }

    std::vector<std::shared_ptr<Track>> tracks = track_library->get_all_matching_tracks(track_name);
    if (tracks.empty()) {
        std::cerr << "[GuildAudioManager] Track '" << track_name << "' not found in library." << std::endl;
        return {};
    }

    if (tracks.size() > 1) {
        std::cout << "[GuildAudioManager] multiple tracks found in library." << std::endl;
        return tracks;
    }

    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        guild_queue->tracks.push_back(tracks.at(0));
    }
    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued track: " << tracks[0]->get_name() << " in guild: " << guild_id << std::endl; 
    return tracks;
}

std::vector<std::shared_ptr<Track>> GuildAudioManager::queue_track_top(dpp::snowflake guild_id, const std::string& track_name) {
    auto guild_queue = get_queue(guild_id);
    if (!guild_queue) {
        std::cerr << "[GuildAudioManager] Failed to get or create queue for guild " << guild_id << std::endl;
        return {};
    }

    std::vector<std::shared_ptr<Track>> tracks = track_library->get_all_matching_tracks(track_name);
    if (tracks.empty()) {
        std::cerr << "[GuildAudioManager] Track '" << track_name << "' not found in library." << std::endl;
        return {};
    }

    if (tracks.size() > 1) {
        std::cout << "[GuildAudioManager] multiple tracks found in library." << std::endl;
        return tracks;
    }

    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        guild_queue->tracks.push_front(tracks.at(0));
    }
    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued track: " << tracks[0]->get_name() << " in guild: " << guild_id << std::endl; 
    return tracks;
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
                guild_queue->tracks.push_back(track);
            } else {
                std::cerr << "[GuildAudioManager] Track '" << track_name << "' not found in library." << std::endl;
            }
        }
    }

    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued all tracks in guild: " << guild_id << std::endl;
}

bool GuildAudioManager::move_track(dpp::snowflake guild_id, size_t from_pos, size_t to_pos) {
    {
        std::lock_guard<std::mutex> lock(manager_mutex);
        auto guild_queue = guild_queues.find(guild_id);
        if (guild_queue == guild_queues.end()) {
            std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
            return false;
        }
    }

    auto track_queue = get_queue(guild_id);

    std::lock_guard<std::mutex> lock(track_queue->queue_mutex);
    size_t queue_size = track_queue->tracks.size();

    if (from_pos >= queue_size || to_pos >= queue_size) {
        std::cerr << "[GuildAudioManager] Invalid positions. Queue size: " << queue_size 
                  << ", from: " << from_pos << ", to: " << to_pos << std::endl;
        return false;
    }

    if (from_pos == to_pos) {
        std::cout << "[GuildAudioManager] from_pos and to_pos are the same. No action taken." << std::endl;
        return true;
    }

    auto from_it = track_queue->tracks.begin() + from_pos;
    auto track = *from_it;
    track_queue->tracks.erase(from_it);
    track_queue->tracks.insert(track_queue->tracks.begin() + to_pos, track);

    std::cout << "[GuildAudioManager] Moved track from position " << from_pos 
              << " to position " << to_pos << " in guild " << guild_id << std::endl;

    cv.notify_all();
    return true;
}

bool GuildAudioManager::remove_track(dpp::snowflake guild_id, size_t position) {
    {
        std::lock_guard<std::mutex> lock(manager_mutex);
        auto guild_queue = guild_queues.find(guild_id);
        if (guild_queue == guild_queues.end()) {
            std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
            return false;
        }
    }
    
    auto track_queue = get_queue(guild_id);

    std::lock_guard<std::mutex> lock(track_queue->queue_mutex);
    size_t queue_size = track_queue->tracks.size();
    
    if (position >= queue_size) {
        std::cerr << "[GuildAudioManager] Invalid positions. Queue size: " << queue_size 
                  << ", position: " << position << std::endl;
        return false;
    }
    auto it = track_queue->tracks.begin() + position;
    track_queue->tracks.erase(it);
    std::cout << "[GuildAudioManager] Removed track at position " << position 
        << " from queue in guild " << guild_id << std::endl;
    return true;
}

bool GuildAudioManager::shuffle(dpp::snowflake guild_id) {
    {
        std::lock_guard<std::mutex> lock(manager_mutex);
        auto guild_queue = guild_queues.find(guild_id);
        if (guild_queue == guild_queues.end()) {
            std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
            return false;
        }
    }

    auto track_queue = get_queue(guild_id);
    
    std::lock_guard<std::mutex> lock(track_queue->queue_mutex);
    size_t queue_size = track_queue->tracks.size();
    if (queue_size < 2) {
        std::cout << "[GuildAudioManager] No need to shuffled queue for guild: " << guild_id << std::endl;
        return true;
    } 
    
    std::shuffle(track_queue->tracks.begin(), track_queue->tracks.end(), g);
    std::cout << "[GuildAudioManager] Shuffled queue for guild: " << guild_id << std::endl;
    return true;
}

bool GuildAudioManager::loop_one(dpp::snowflake guild_id, bool loop_one) {
    {
        std::lock_guard<std::mutex> lock(manager_mutex);
        auto guild_queue = guild_queues.find(guild_id);
        if (guild_queue == guild_queues.end()) {
            std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
            return false;
        }
    }
    auto track_queue = get_queue(guild_id);
    
    std::lock_guard<std::mutex> lock(track_queue->queue_mutex);
    track_queue->loop_one = loop_one;
    track_queue->loop_all = false;
    std::cout << "[GuildAudioManager] set looping of queue to: " << loop_one << " for guild: " << guild_id << std::endl;
    return true;
}

bool GuildAudioManager::loop_all(dpp::snowflake guild_id, bool loop_all) {
    {
        std::lock_guard<std::mutex> lock(manager_mutex);
        auto guild_queue = guild_queues.find(guild_id);
        if (guild_queue == guild_queues.end()) {
            std::cerr << "[GuildAudioManager] No queue found for guild " << guild_id << std::endl;
            return false;
        }
    }
    auto track_queue = get_queue(guild_id);
    
    std::lock_guard<std::mutex> lock(track_queue->queue_mutex);
    track_queue->loop_all = loop_all;
    track_queue->loop_one = false;
    std::cout << "[GuildAudioManager] set looping of queue to: " << loop_all << " for guild: " << guild_id << std::endl;
    return true;
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
            audio->pause_audio(guild_id);
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
            audio->resume_audio(guild_id);
            std::cout << "[GuildAudioManager] Resumed track for guild " << guild_id << std::endl;
            cv.notify_all();
        }
    }
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

                    if (!guild_queue->is_playing && (!guild_queue->tracks.empty() || guild_queue->loop_all)) {
                        if (!audio->voice_ready(guild_id)) {
                            it++;
                            continue;
                        }
                        if (guild_queue->tracks.empty()) {
                            for (auto track : guild_queue->tracks_played) {
                                guild_queue->tracks.push_front(track);
                            }
                            guild_queue->tracks_played.clear();
                        }
                        std::shared_ptr<Track> track = guild_queue->tracks.front();
                        guild_queue->current_track = track;
                        if (!guild_queue->loop_one) {
                            guild_queue->tracks.pop_front();
                        }
                        guild_queue->is_playing = true;
                        guild_queue->skip = false;
                        guild_queue->elapsed = 0;
                        std::cout << "[GuildAudioManager] playing track '" << guild_queue->current_track->get_name()
                            << "' for guild " << guild_id << std::endl;
                        std::thread([this, guild_queue, track, guild_id]() {
                            auto stop_callback = [guild_queue]() {
                                std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
                                return guild_queue->skip || guild_queue->stop;
                            };

                            auto pause_callback = [guild_queue]() {
                                std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
                                return guild_queue->paused;
                            };

                            audio->send_audio_to_voice(guild_id, track, stop_callback, pause_callback);
                            
                            /// TODO WAIT FOR AUDIO TO TRULY PLAY BEFORE STARTING COUNTDOWN

                            size_t length_s = track->get_length();
                            size_t elapsed_s = 0;
                            while (elapsed_s < length_s) {
                                std::this_thread::sleep_for(std::chrono::seconds(1));
                                {
                                    std::unique_lock<std::mutex> lock(guild_queue->queue_mutex);
                                    if (guild_queue->paused) {
                                        cv.wait(lock, [&guild_queue]() {
                                            return !guild_queue->paused || guild_queue->stop || guild_queue->skip;
                                        });
                                    }
                                    if (guild_queue->skip || guild_queue->stop) {
                                        std::cout << "[GuildAudioManager] Track skipped or stop requested: " << track->get_name() << std::endl;
                                        guild_queue->is_playing = false;
                                        audio->stop_audio(guild_id);
                                        cv.notify_all();
                                        guild_queue->tracks_played.push_front(track);
                                        track->unload();
                                        return;
                                    }
                                    guild_queue->elapsed = elapsed_s;
                                }
                                elapsed_s += 1;
                            }

                            if (guild_queue) {
                                std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
                                if (!guild_queue->stop) {
                                    guild_queue->is_playing = false;
                                }
                            }
                            std::cout << "[GuildAudioManager] Finished playing track " << track->get_name() << std::endl;
                            guild_queue->tracks_played.push_front(track);
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

std::string format_seconds(size_t total_seconds) {
    size_t hours = total_seconds / 3600;
    size_t minutes = (total_seconds % 3600) / 60;
    size_t seconds = total_seconds % 60;

    std::ostringstream oss;

    if (hours > 0) {
        oss << hours << ":"
            << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << seconds;
    } else {
        oss << minutes << ":"
            << std::setw(2) << std::setfill('0') << seconds;
    }

    return oss.str();
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
            std::string current = "";
            current += queue->current_track->get_name();
            current += " (" + format_seconds(queue->elapsed);
            current += "/" + format_seconds(queue->current_track->get_length()) + ")";
            tracks.push_back(current);
        }

        std::deque<std::shared_ptr<Track>> temp_queue = queue->tracks;
        while (!temp_queue.empty()) {
            if (temp_queue.front()) {
                tracks.push_back(temp_queue.front()->toString());
            } else {
                std::cerr << "[GuildAudioManager] Encountered a null track in the queue for guild " << guild_id << std::endl;
                tracks.push_back("Unknown Track");
            }
            temp_queue.pop_front();
        } 
    }

    return tracks;

}
