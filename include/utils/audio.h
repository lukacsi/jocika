#ifndef AUDIO_H
#define AUDIO_H

#include "utils/track_library.h"
#include <dpp/discordclient.h>
#include <dpp/snowflake.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>


class Audio {
public:
    Audio(std::shared_ptr<TrackLibrary> _track_library);

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    void send_audio_to_voice(dpp::snowflake guild_id, const std::string& track_name);
    void stop_audio(dpp::snowflake guild_id);

    void set_voice_connection(dpp::snowflake guild_id, dpp::voiceconn* vc);
    dpp::voiceconn* get_voice_connection(dpp::snowflake guild_id);
    void remove_voice_connection(dpp::snowflake guild_id);

    void join_voice(dpp::guild* guild, dpp::snowflake user_id);
    void disconnect_voice(dpp::snowflake guild_id);

private:
    std::shared_ptr<TrackLibrary> track_library;

    std::map<dpp::snowflake, dpp::voiceconn*> voice_connections;
    std::mutex voice_mutex;
};


#endif
