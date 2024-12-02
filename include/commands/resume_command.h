#ifndef RESUME_COMMAND_H
#define RESUME_COMMAND_H

#include "voice_command.h"

class ResumeCommand : public VoiceCommand {
public:
    ResumeCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("resume", "Resume audio playback", _audio_processor, _guild_audio_manager) {}
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
};

#endif
