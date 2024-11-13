#include "utils/audio.h"
#include "utils/guild_audio_manager.h"
#include "utils/track_library.h"
#include <dpp/discordclient.h>
#include <dpp/discordvoiceclient.h>
#include <opus/opus.h>
#include <cstdint>
#include <memory>
#include <mpg123.h>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

Audio::Audio(std::shared_ptr<TrackLibrary> _track_library) 
    : track_library(_track_library) {}


void Audio::send_audio_to_voice(dpp::snowflake guild_id, const std::string& track_name) {
    auto pcm_data = track_library->get_track(track_name)->get_pcm_data();
    if (pcm_data.empty()) {
        std::cerr << "[Audio] Track not found: " << track_name << std::endl;
        return;
    }
    auto vc = get_voice_connection(guild_id);
    if (!vc) {
        std::cerr << "[Audio] Guild audio connection not found for " << guild_id << std::endl;
        return;
    }
    vc->voiceclient->send_audio_raw((uint16_t*)pcm_data.data(), pcm_data.size());
} 

void Audio::join_voice(dpp::guild* guild, dpp::snowflake user_id) {
    if (!guild->connect_member_voice(user_id)) {
        std::cout << "[Audio] User is not in a voice channel " << user_id << std::endl;
    }
}

void Audio::disconnect_voice(dpp::snowflake guild_id) {
    auto vc = voice_connections.find(guild_id)->second;
    if (!vc) {
        std::cerr << "[Audio] Not in a voice channel in " << guild_id << std::endl;
        return;
    }
    vc->disconnect();
    remove_voice_connection(guild_id);
    std::cout << "[Audio] Disconnected form voice in " << guild_id << std::endl;

}

void Audio::stop_audio(dpp::snowflake guild_id) {
    auto vc = get_voice_connection(guild_id);
    if (!vc) {
        std::cerr << "[Audio] No active voice connections for guild " << guild_id << std::endl;
        return;
    }

    vc->voiceclient->stop_audio();
    std::cout << "[Audio] stopped playback for " << guild_id << std::endl;
}

void Audio::set_voice_connection(dpp::snowflake guild_id, dpp::voiceconn* vc) {
    std::lock_guard<std::mutex> lock(voice_mutex);
    voice_connections[guild_id] = vc;
    std::cout << "[Audio] Set voice connection for guild " << guild_id << std::endl;
}

dpp::voiceconn* Audio::get_voice_connection(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(voice_mutex);
    auto it = voice_connections.find(guild_id);
    if (it != voice_connections.end()) {
        return it->second;
    }
    return nullptr;
}

void Audio::remove_voice_connection(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(voice_mutex);
    auto it = voice_connections.find(guild_id);
    if (it != voice_connections.end()) {
        if (it->second) {
            it->second->disconnect();
        }
        voice_connections.erase(it);
        std::cout << "[Audio] Removed voice connection for guild " << guild_id << std::endl;
    }
}
