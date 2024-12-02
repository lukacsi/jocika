#ifndef DISCONNECT_COMMAND_H
#define DISCONNECT_COMMAND_H

#include "voice_command.h"


class DisconnectCommand: public VoiceCommand {
public:
    DisconnectCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager)
    : VoiceCommand("disconnect", "Disconnects form voice", _audio_processor, _guild_audio_manager) {}
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
};

#endif
