#include "utils/audio.h"
#include "utils/guild_audio_manager.h"
#include <filesystem>
#include <memory>
#include <mpg123.h>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

Audio::Audio(std::shared_ptr<GuildAudioManager> man) : guild_audio_manager(man) {
    if (mpg123_init() != MPG123_OK) {
        std::cerr << "Failed to initialize mpg123" << std::endl;
    }
}

Audio::~Audio() {
    mpg123_exit();
}

bool Audio::decode_mp3(const std::string& file_path, std::vector<int16_t>& pcm_data) {
    mpg123_handle* mh = mpg123_new(NULL, NULL);
    if (!mh) {
        std::cerr << "Failed to create mpg123 handle." << std::endl;
        return false;
    }

    mpg123_format_none(mh);
    mpg123_format(mh, 48000, MPG123_STEREO, MPG123_ENC_SIGNED_16);

    if (mpg123_open(mh, file_path.c_str()) != MPG123_OK) {
        std::cerr << "Failed to open file: " << file_path << mpg123_strerror(mh) << std::endl;
        mpg123_delete(mh);
        return false;
    }

    size_t buffer_size = mpg123_outblock(mh);
    std::vector<unsigned char> buffer(buffer_size);
    size_t done = 0;

    while (mpg123_read(mh, buffer.data(), buffer.size(), &done) == MPG123_OK) {
        int16_t* samples = reinterpret_cast<int16_t*>(buffer.data());
        size_t sample_count  = done / sizeof(int16_t);
        pcm_data.insert(pcm_data.end(), samples, samples + sample_count);
    }

    if (mpg123_close(mh) != MPG123_OK) {
        std::cerr << "Failed to close file: " << file_path << std::endl;
        mpg123_delete(mh);
        return false;
    }
    
    mpg123_delete(mh);
    return true;
}

bool Audio::load_audio_files(const std::string& media_dir) {
    std::lock_guard<std::mutex> lock(tracks_mutex);

    for (const auto& entry : std::filesystem::directory_iterator(media_dir)) {
        if(entry.is_regular_file() && entry.path().extension() == ".mp3") {
            std::string track_name = entry.path().stem().string();
            std::vector<int16_t> pcm_data;

            if (decode_mp3(entry.path().string(), pcm_data)) {
                tracks[track_name] = std::move(pcm_data);
                std::cout << "Loaded track: " << track_name << std::endl;
            } else {
                std::cerr << "Failed to load track: " << entry.path().string() << std::endl;
            }
        }
    }

    return !tracks.empty();
}

bool Audio::get_track_pcm(const std::string& track_name, std::vector<int16_t>& pcm_data) const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    auto it = tracks.find(track_name);
    if (it != tracks.end()) {
        pcm_data = it->second;
        return true;
    }
    return false;
}

std::vector<std::string> Audio::get_all_track_names() const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    std::vector<std::string> track_names;
    for (const auto& [name, _] : tracks) {
        track_names.push_back(name);
    }
    return track_names;
}

// Play a specific track in the given guild
void Audio::play_track(dpp::snowflake guild_id, const std::string& track_name) {
    std::vector<int16_t> pcm_data;
    if (!get_track_pcm(track_name, pcm_data)) {
        std::cerr << "Track not found: " << track_name << std::endl;
        return;
    }

    guild_audio_manager->queue_track(guild_id, track_name);

    // Assuming you have access to the voice connection
    // Since playback is managed by GuildAudioManager, the actual sending is handled there
    // Here, you might trigger the sending if needed

    std::cout << "Playing track '" << track_name << "' for guild " << guild_id << std::endl;
    // Actual sending is handled in GuildAudioManager's playback loop
}
