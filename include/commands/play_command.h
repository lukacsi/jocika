#ifndef PLAY_COMMAND_H
#define PLAY_COMMAND_H

#include "voice_command.h"
#include <memory>

class PlayCommand : public VoiceCommand {
public:
    PlayCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("play", "Plays mp3 in voice channel.", _audio_processor, _guild_audio_manager)  {
        add_option(dpp::command_option(dpp::co_string, "file", "Specify the file name to play", false));
    }
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
private:
};

#endif
