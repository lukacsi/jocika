#ifndef QUEUE_COMMAND_H
#define QUEUE_COMMAND_H

#include "voice_command.h"

class QueueCommand : public VoiceCommand {
public:
    QueueCommand(std::shared_ptr<Audio> _audio_processor, std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
    : VoiceCommand("queue", "Displays the current queue", _audio_processor, _guild_audio_manager) {
        auto list = dpp::command_option(dpp::co_sub_command, "list", "List track in queue");
        auto move = dpp::command_option(dpp::co_sub_command, "move", "Move tracks around in queue by position")
            .add_option(dpp::command_option(dpp::co_integer, "from", "Track to move, by queue position", true))
            .add_option(dpp::command_option(dpp::co_integer, "to", "Queue postion to move the track to", true));
        auto remove = dpp::command_option(dpp::co_sub_command, "remove", "Remove a track from the queue by position")
            .add_option(dpp::command_option(dpp::co_integer, "position", "Position of the removable track", true));
        add_option(list);
        add_option(move);
        add_option(remove);
    }
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;

};

#endif
