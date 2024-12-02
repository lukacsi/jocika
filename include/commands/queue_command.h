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
        auto shuffle = dpp::command_option(dpp::co_sub_command, "shuffle", "Shuffles the queue");
        auto loop_one = dpp::command_option(dpp::co_sub_command, "loop_track", "Loop the current track")
            .add_option(dpp::command_option(dpp::co_boolean, "loop", "Loop the track", true));
        auto loop_all = dpp::command_option(dpp::co_sub_command, "loop_queue", "Loop the queue")
            .add_option(dpp::command_option(dpp::co_boolean, "loop", "Loop queue", true));
        add_option(list);
        add_option(move);
        add_option(remove);
        add_option(shuffle);
        add_option(loop_one);
        add_option(loop_all);
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;

};

#endif
