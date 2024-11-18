#ifndef CLEAR_COMMAND_H
#define CLEAR_COMMAND_H

#include "voice_command.h"

class ClearCommand : public VoiceCommand {
public:
    ClearCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("clear", "Clears the queue", _audio_processor, _guild_audio_manager) {}
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
};

#endif
