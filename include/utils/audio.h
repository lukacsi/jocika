#ifndef AUDIO_H
#define AUDIO_H

#include "dpp/dpp.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class GuildAudioManager;

class Audio {
public:
    Audio(std::shared_ptr<GuildAudioManager>);
    ~Audio();

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    bool load_audio_files(const std::string& media_dir);
    bool get_track_pcm(const std::string& track_name, std::vector<int16_t>& pcm_data) const;

    std::vector<std::string> get_all_track_names() const;

    void play_track(dpp::snowflake guild_id, const std::string& track_name);

private:
    std::shared_ptr<GuildAudioManager> guild_audio_manager;
    std::map<std::string, std::vector<int16_t>> tracks;
    mutable std::mutex tracks_mutex;
    bool decode_mp3(const std::string& file_path, std::vector<int16_t>& pcm_data);
};


#endif
