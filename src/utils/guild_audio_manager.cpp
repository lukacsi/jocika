#include "utils/guild_audio_manager.h"
#include "utils/audio.h"
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <vector>

GuildAudioManager::GuildAudioManager(std::shared_ptr<Audio> audio)
    : audio(audio), stop_thread(false) {
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
    std::cout << "Queued track: " << track_name << " in guild: " << guild_id << std::endl;
}

void GuildAudioManager::queue_all(dpp::snowflake guild_id) {
    GuildQueue* guild_queue = get_queue(guild_id);
    std::vector<std::string> all_tracks = audio->get_all_track_names();
    {
        std::lock_guard<std::mutex> lock(guild_queue->queue_mutex);
        for (const auto& track : all_tracks) {
            guild_queue->tracks.push(track);
        }
    }
    cv.notify_all();
    std::cout << "Queued all tracks in guild:  " << guild_id << std::endl;
}

void GuildAudioManager::playback_loop() {
    while (true) {
        std::unique_lock<std::mutex> lock(cv_mutex):
        cv.wait(lock, [this]() )
    } 
}
