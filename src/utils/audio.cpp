#include "utils/audio.h"
#include "utils/guild_audio_manager.h"
#include "utils/process.h"
#include "utils/track_library.h"
#include <array>
#include <cstdio>
#include <dpp/discordclient.h>
#include <dpp/discordvoiceclient.h>
#include <dpp/dispatcher.h>
#include <dpp/snowflake.h>
#include <opus/opus.h>
#include <cstdint>
#include <memory>
#include <mpg123.h>
#include <iostream>
#include <ostream>
#include <vector>

Audio::Audio() {}

bool Audio::voice_ready(dpp::snowflake guild_id) {
    std::lock_guard<std::mutex> lock(voice_mutex);
    auto it = voice_connections.find(guild_id);
    if (it == voice_connections.end() || !it->second) {
        std::cerr << "[Audio] No active voice connections for guild " << guild_id << std::endl;
        return false;
    }

    if (!it->second->is_active() ) {
        std::cerr << "[Audio] Voice connection for guild " << guild_id << " is not ready." << std::endl;
        return false;
    }

    if (!it->second->voiceclient->is_ready()) {
        std::cerr << "[Audio] Voice client for guild " << guild_id << " is not ready." << std::endl;
        return false;
    }

    std::cout << "[Audio] Voice connection for guild " << guild_id << " is ready." << std::endl;
    return true;
}

void Audio::send_audio_to_voice(dpp::snowflake guild_id, std::shared_ptr<Track> track,
                                std::function<bool()> stop_callback, std::function<bool()> pause_callback) {
    if (!track) {
        std::cerr << "[Audio] Null track provided to send_audio_to_voice." << std::endl;
        return;
    }

    auto vc = get_voice_connection(guild_id);
    if (!vc) {
        std::cerr << "[Audio] Guild audio connection not found for " << guild_id << std::endl;
        return;
    }
    if (track->get_source_type() == SourceType::Youtube) {
        std::thread([this, vc, track, stop_callback, pause_callback]() {
            send_stream_audio(vc, track->get_source(), stop_callback, pause_callback);
        }).detach();
    } else {
        // Handle local files with pause and resume
        send_local_audio(vc, track, stop_callback, pause_callback);
    }
} 

void Audio::send_stream_audio(dpp::voiceconn* vc, const std::string& url,
                                  std::function<bool()> stop_callback, std::function<bool()> pause_callback) {
    std::vector<std::string> command = {
        "bash", "-c",
        "yt-dlp -f bestaudio -o - \"" + url + "\" | ffmpeg -i pipe:0 -map 0:a -c:a copy -f opus -loglevel quiet pipe:1"
    };

    Process process(command);
    if (!process.start()) {
        std::cerr << "[Audio] Failed to start process for streaming audio." << std::endl;
        return;
    }

    std::array<uint8_t, 4096> buffer;
    bool paused = false;

    while (true) {
        if (stop_callback()) {
            std::cout << "[Audio] Stopping streaming as per control callback." << std::endl;
            break;
        }

        if (pause_callback()) {
            if (!paused) {
                process.stop();
                paused = true;
                std::cout << "[Audio] Paused streaming." << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        } else {
            if (paused) {
                process.resume();
                paused = false;
                std::cout << "[Audio] Resumed streaming." << std::endl;
            }
        }

        ssize_t bytes_read = process.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        if (bytes_read <= 0) {
            break;  // End of stream or error
        }

        try {
            vc->voiceclient->send_audio_opus(buffer.data(), bytes_read, 20);
        } catch (const dpp::voice_exception& e) {
            std::cerr << "[Audio] Error sending audio: " << e.what() << std::endl;
            break;
        }
    }

    process.terminate();
    vc->voiceclient->stop_audio();    
}

void Audio::send_local_audio(dpp::voiceconn* vc, std::shared_ptr<Track> track,
                             std::function<bool()> stop_callback,
                             std::function<bool()> pause_callback) {
    track->load();
    auto pcm_data = track->get_pcm_data();
    if (pcm_data.empty()) {
        std::cerr << "[Audio] Track has no PCM data: " << track->get_name() << std::endl;
        return;
    }
    vc->voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(pcm_data.data()), pcm_data.size());
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

void Audio::pause_audio(dpp::snowflake guild_id) {
    auto vc = get_voice_connection(guild_id);
    if (!vc) {
        std::cerr << "[Audio] No active voice connections for guild " << guild_id << std::endl;
        return;
    }

    vc->voiceclient->pause_audio(true);
    std::cout << "[Audio] Paused playback for " << guild_id << std::endl;
}

void Audio::resume_audio(dpp::snowflake guild_id) {
    auto vc = get_voice_connection(guild_id);
    if (!vc) {
        std::cerr << "[Audio] No active voice connections for guild " << guild_id << std::endl;
        return;
    }

    vc->voiceclient->pause_audio(false);
    std::cout << "[Audio] Resumed playback for " << guild_id << std::endl;
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

/*void Audio::recover_voice_connections(std::vector<dpp::snowflake> guild_ids) {
}*/
