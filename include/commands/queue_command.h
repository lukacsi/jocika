#ifndef QUEUE_COMMAND_H
#define QUEUE_COMMAND_H

#include "voice_command.h"

class QueueCommand : public VoiceCommand {
public:
    QueueCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("queue", "Displays the current queue", _audio_processor, _guild_audio_manager) {}
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;

};

#endif
