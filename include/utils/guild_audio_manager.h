#ifndef GUILD_AUDIO_MANAGER_H
#define GUILD_AUDIO_MANAGER_H

#include "utils/track_library.h"
#include <condition_variable>
#include <dpp/discordclient.h>
#include <dpp/snowflake.h>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class Audio;

struct GuildQueue {
    std::queue<std::string> tracks;
    bool is_playing;
    std::mutex queue_mutex;
    dpp::voiceconn* voice_connection;

    GuildQueue() : is_playing(false), voice_connection(nullptr) {}
};

class GuildAudioManager {
public:
    GuildAudioManager(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<TrackLibrary> _track_library);
    ~GuildAudioManager();

    GuildAudioManager(const GuildAudioManager&) = delete;
    GuildAudioManager& operator=(const GuildAudioManager&) = delete;

    void queue_track(dpp::snowflake guild_id, const std::string& track_name);
    void queue_all(dpp::snowflake guild_id);
    void skip_track(dpp::snowflake guild_id);

    void set_voice_connection(dpp::snowflake guild_id, dpp::voiceconn* vc);

private:
    std::map<dpp::snowflake, std::unique_ptr<GuildQueue>> guild_queues;
    std::mutex manager_mutex;

    std::shared_ptr<Audio> audio;
    std::shared_ptr<TrackLibrary> track_library;

    std::condition_variable cv;
    std::mutex cv_mutex;
    bool stop_thread;

    std::thread playback_thread;

    void playback_loop();

    GuildQueue* get_queue(dpp::snowflake guild_id);
};

#endif
