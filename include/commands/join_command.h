#ifndef JOIN_COMMAND_H
#define JOIN_COMMAND_H

#include "voice_command.h"

class JoinCommand : public VoiceCommand {
public:
    JoinCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("join", "Joins your voice channel", _audio_processor, _guild_audio_manager) {}
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
};

#endif
