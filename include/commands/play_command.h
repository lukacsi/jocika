#ifndef PLAY_COMMAND_H
#define PLAY_COMMAND_H

#include "voice_command.h"
#include <dpp/appcommand.h>
#include <memory>

class PlayCommand : public VoiceCommand {
public:
    PlayCommand(std::shared_ptr<Audio> _audio_processor,
                std::shared_ptr<GuildAudioManager> _guild_audio_manager,
                std::shared_ptr<TrackLibrary> _track_library) 
    : VoiceCommand("play", "Plays mp3 in voice channel.", _audio_processor, _guild_audio_manager)  {
        track_library = _track_library;
        
        auto top_option = dpp::command_option(dpp::co_boolean, "top", "Place at the top of the queue", false);

        auto track = dpp::command_option(dpp::co_sub_command, "from_library", "Specify a track by name from the library")
            .add_option(dpp::command_option(dpp::co_string, "name", "Name of the track", true))
            .add_option(top_option);
        auto like = dpp::command_option(dpp::co_sub_command, "like", "Queue all tracks from library that match keyword")
            .add_option(dpp::command_option(dpp::co_string, "keyword", "Search keyword", true))
            .add_option(top_option);
        auto url = dpp::command_option(dpp::co_sub_command, "from_youtube", "Queue a track from youtube by url")
            .add_option(dpp::command_option(dpp::co_string, "url", "YouTube url", true))
            .add_option(top_option);
        auto all = dpp::command_option(dpp::co_sub_command, "all", "Queue all tracks in library");

        add_option(url);
        add_option(track);
        add_option(like);
        add_option(all);
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
private:
    std::shared_ptr<TrackLibrary> track_library;
};

#endif
