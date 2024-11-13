#ifndef AUDIO_H
#define AUDIO_H

#include <dpp/discordclient.h>
#include <dpp/snowflake.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class GuildAudioManager;
class TrackLibrary;

class Audio {
public:
    Audio(std::shared_ptr<TrackLibrary> _track_library);

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    std::vector<std::string> get_all_track_names() const;

    void play_track(dpp::snowflake guild_id, const std::string& track_name, dpp::voiceconn* vc);

    void send_audio_to_voice(dpp::voiceconn* vc, const std::string& track_name);

    void set_guild_audio_manager(std::shared_ptr<GuildAudioManager> manager);

    bool add_track(const std::string& name, const std::string& file_path);

    bool remove_track(const std::string& name);

private:
    std::shared_ptr<TrackLibrary> track_library;
    std::shared_ptr<GuildAudioManager> guild_audio_manager;

    std::map<std::string, std::vector<uint8_t>> tracks;
    mutable std::mutex tracks_mutex;
    bool decode_mp3(const std::string& file_path, std::vector<uint8_t>& pcm_data);
};


#endif
