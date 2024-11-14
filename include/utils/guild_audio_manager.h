#ifndef GUILD_AUDIO_MANAGER_H
#define GUILD_AUDIO_MANAGER_H

#include "utils/track.h"
#include "utils/track_library.h"
#include <condition_variable>
#include <dpp/discordclient.h>
#include <dpp/snowflake.h>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class Audio;

struct GuildQueue {
    std::queue<std::shared_ptr<Track>> tracks;
    std::shared_ptr<Track> current_track;
    bool is_playing;
    bool stop;
    bool skip;
    bool paused;
    std::mutex queue_mutex;

    GuildQueue() : is_playing(false), stop(false), skip(false), paused(false), current_track(nullptr) {}
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
    void pause_track(dpp::snowflake guild_id);
    void resume_track(dpp::snowflake guild_id);

    //void remove_queue(dpp::snowflake guild_id);
    void clear_queue(dpp::snowflake guild_id);

    std::vector<std::string> get_queued_tracks(dpp::snowflake guild_id);

    

private:
    std::map<dpp::snowflake, std::shared_ptr<GuildQueue>> guild_queues;
    std::mutex manager_mutex;

    std::shared_ptr<Audio> audio;
    std::shared_ptr<TrackLibrary> track_library;

    std::condition_variable cv;
    std::mutex cv_mutex;
    bool stop_thread;

    std::thread playback_thread;

    void playback_loop();

    std::shared_ptr<GuildQueue> get_queue(dpp::snowflake guild_id);
};

#endif
