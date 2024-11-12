#ifndef GUILD_AUDIO_MANAGER_H
#define GUILD_AUDIO_MANAGER_H

#include <condition_variable>
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

    GuildQueue() : is_playing(false) {}
};

class GuildAudioManager {
public:
    GuildAudioManager(std::shared_ptr<Audio> audio_processor);
    ~GuildAudioManager();

    GuildAudioManager(const GuildAudioManager&) = delete;
    GuildAudioManager& operator=(const GuildAudioManager&) = delete;

    void queue_track(dpp::snowflake guild_id, const std::string& stack_name);
    void queue_all(dpp::snowflake guild_id);

private:
    std::map<dpp::snowflake, std::unique_ptr<GuildQueue>> guild_queues;
    std::mutex guild_queue_mutex;

    std::shared_ptr<Audio> audio;

    std::condition_variable cv;
    std::mutex cv_mutex;
    bool stop_thread;

    std::thread playback_thread;

    void playback_loop();

    GuildQueue* get_queue(dpp::snowflake guild_id);
};

#endif
