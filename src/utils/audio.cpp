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

std::vector<std::string> Audio::get_all_track_names() const {
    return track_library->get_all_track_names();
}

void Audio::play_track(dpp::snowflake guild_id, const std::string& track_name, dpp::voiceconn* vc) {
    std::vector<uint8_t> pcm_data;
    if (!track_library->get_track(track_name)) {
        std::cerr << "[Audio] Track not found: " << track_name << std::endl;
        return;
    }
        
    guild_audio_manager->set_voice_connection(guild_id, vc);
    guild_audio_manager->queue_track(guild_id, track_name);

    std::cout << "[Audio] Enqued track '" << track_name << "' for guild " << guild_id << std::endl;
}

void Audio::send_audio_to_voice(dpp::voiceconn* vc, const std::string& track_name) {
    auto pcm_data = track_library->get_track(track_name)->get_pcm_data();
    if (pcm_data.empty()) {
        std::cerr << "[Audio] Track not found: " << track_name << std::endl;
        return;
    }
    vc->voiceclient->send_audio_raw((uint16_t*)pcm_data.data(), pcm_data.size());
} 

void Audio::set_guild_audio_manager(std::shared_ptr<GuildAudioManager> manager) {
    guild_audio_manager = manager;
}
