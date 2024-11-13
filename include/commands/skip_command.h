#ifndef SKIP_COMMAND_H
#define SKIP_COMMAND_H

#include "voice_command.h"
#include <memory>

class SkipCommand : public VoiceCommand {
public:
    SkipCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("skip", "Skips the currently playing song.", _audio_processor, _guild_audio_manager) {}
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
private:
    std::shared_ptr<Audio> audio;
};

#endif
