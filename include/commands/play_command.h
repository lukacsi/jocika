#ifndef PLAY_COMMAND_H
#define PLAY_COMMAND_H

#include "voice_command.h"
#include <memory>

class PlayCommand : public VoiceCommand {
public:
    PlayCommand(std::shared_ptr<Audio> _audio_processor,
                std::shared_ptr<GuildAudioManager> _guild_audio_manager,
                std::shared_ptr<TrackLibrary> _track_library) 
    : VoiceCommand("play", "Plays mp3 in voice channel.", _audio_processor, _guild_audio_manager)  {
        track_library = _track_library;
        auto file_option = dpp::command_option(dpp::co_string, "file", "Specify the file name to play", false);
        auto loose_option = dpp::command_option(dpp::co_string, "like", "Place all tracks in queue matching a keyword", false);
        auto url_option = dpp::command_option(dpp::co_string, "url", "Youtube url to stream from", false);
        auto top_option = dpp::command_option(dpp::co_boolean, "top", "Place at the top of the queue", false);
        add_option(url_option);
        add_option(file_option);
        add_option(loose_option);
        add_option(top_option);
    }
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
private:
    std::shared_ptr<TrackLibrary> track_library;
};

#endif
