#ifndef AUDIO_H
#define AUDIO_H

#include "track.h"
#include <dpp/discordclient.h>
#include <dpp/snowflake.h>
#include <map>
#include <mutex>

class Audio {
public:
    Audio();

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;
    
    //void recover_voice_connections(std::vector<dpp::snowflake> guild_ids);

    bool voice_ready(dpp::snowflake guild_id);
    void send_audio_to_voice(dpp::snowflake guild_id, std::shared_ptr<Track> track);
    void stop_audio(dpp::snowflake guild_id);

    void set_voice_connection(dpp::snowflake guild_id, dpp::voiceconn* vc);
    dpp::voiceconn* get_voice_connection(dpp::snowflake guild_id);
    void remove_voice_connection(dpp::snowflake guild_id);

    void join_voice(dpp::guild* guild, dpp::snowflake user_id);
    void disconnect_voice(dpp::snowflake guild_id);

private:
    std::map<dpp::snowflake, dpp::voiceconn*> voice_connections;
    std::mutex voice_mutex;
};

#endif
