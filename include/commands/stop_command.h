#ifndef STOP_COMMAND_H
#define STOP_COMMAND_H

#include "voice_command.h"

class StopCommand : public VoiceCommand {
public:
    StopCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("stop", "Stops audio playback and clears the queue", _audio_processor, _guild_audio_manager) {}   
    void execute(const dpp::slashcommand_t &event, const dpp::cluster &bot) override;
};


#endif
