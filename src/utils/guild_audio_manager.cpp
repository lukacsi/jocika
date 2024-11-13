#include "utils/guild_audio_manager.h"
#include "utils/audio.h"
#include "utils/track_library.h"
#include <chrono>
#include <dpp/snowflake.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
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

void GuildAudioManager::queue_track(dpp::snowflake guild_id, const std::string& track_name) {
    GuildQueue* guild_queue = get_queue(guild_id);
    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        guild_queue->tracks.push(track_name);
    }
    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued track: " << track_name << " in guild: " << guild_id << std::endl;
}

void GuildAudioManager::queue_all(dpp::snowflake guild_id) {
    GuildQueue* guild_queue = get_queue(guild_id);
    std::vector<std::string> all_tracks = track_library->get_all_track_names();
    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        for (const auto& track : all_tracks) {
            guild_queue->tracks.push(track);
        }
    }
    cv.notify_all();
    std::cout << "[GuildAudioManager] Queued all tracks in guild:  " << guild_id << std::endl;
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

                // Check if exit was requested
                {
                    std::lock_guard<std::mutex> queue_lock(guild_queue->queue_mutex);
                    if (guild_queue->stop) {
                        std::cout << "[GuildAudioManager] Exit requested for guild " << guild_id << std::endl;
                        it = guild_queues.erase(it); // Remove the queue and exit the loop
                        continue;
                    }
                }

                {
                    std::lock_guard<std::mutex> queue_lock(guild_queue->queue_mutex);

                    if (!guild_queue->is_playing && !guild_queue->tracks.empty()) {
                        std::string track = guild_queue->tracks.front();
                        guild_queue->tracks.pop();
                        guild_queue->is_playing = true;

                        // Send audio in a detached thread
                        std::thread([this, guild_queue, track, guild_id]() {
                            audio->send_audio_to_voice(guild_id, track);
                            std::this_thread::sleep_for(std::chrono::milliseconds(track_library->get_length_ms(track) + 1000));

                            if (guild_queue) {
                                std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
                                if (!guild_queue->stop) {
                                    guild_queue->is_playing = false;
                                }
                            }

                            std::cout << "[GuildAudioManager] Finished playing track " << track << std::endl;
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

GuildQueue* GuildAudioManager::get_queue(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(manager_mutex);
    auto it = guild_queues.find(guild_id);
    if (it == guild_queues.end()) {
        auto guild_queue = std::make_shared<GuildQueue>();
        GuildQueue* ptr = guild_queue.get();
        guild_queues[guild_id] = std::move(guild_queue);
        return ptr;
    }
    return it->second.get();
}
