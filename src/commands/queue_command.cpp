#include "commands/queue_command.h"
#include "globals.h"
#include <string>

void QueueCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    auto cmd_data = event.command.get_command_interaction();
    if (!guild_audio_manager) {
        std::cerr << "[QueueCommand] guild_audio_manager is not initialized." << std::endl;
        event.reply("Internal error: Guild audio manager not initialized.");
        return;
    }

    auto guild_id = event.command.guild_id;
    auto subcommand = cmd_data.options[0];
    if (subcommand.name == "list") {
        std::vector<std::string> tracks = guild_audio_manager->get_queued_tracks(guild_id);

        if (tracks.empty()) {
            event.reply("Queue is currently empty.");
            return;
        }

        std::string reply_message = "Current Track: " + tracks[0] + '\n';
        int i = max_tracks_reply;
        if (tracks.size() > 1) {
            reply_message += "Tracks queued:\n";
            for (size_t i = 1; i < tracks.size(); ++i) {
                if (i > 0) {
                    i--;
                    reply_message += std::to_string(i) + ". " + tracks[i] + "\n";
                }
            }
            if (i == 0) {
                event.reply(std::to_string(tracks.size() - max_tracks_reply) + " more tracks \n");
            }
        }
        
        event.reply(reply_message);
    }
    else if (subcommand.name == "move") {
        size_t from, to;
        for (int i = 0; i < 2; i++) {
            auto option = subcommand.options[i];
            if (option.name == "from") {
                from = subcommand.get_value<int64_t>(i);
            } else if (option.name == "to") {
                to = subcommand.get_value<int64_t>(i);
            }
        }
        if (!guild_audio_manager->move_track(guild_id, from - 1, to - 1)) {
            event.reply("Failed to move track in positon " + std::to_string(from) +
                        " to position " + std::to_string(to));
            return;
        }
        std::string reply = "Moved track in positon " + std::to_string(from) 
            + " to position " + std::to_string(to);
        event.reply(reply);
    } else if (subcommand.name == "remove") {
        size_t position = subcommand.get_value<int64_t>(0);
        if (!guild_audio_manager->remove_track(guild_id, position - 1)) {
            event.reply("Failed to remove track in position: " + std::to_string(position)
                        + " from queue");
        }
        event.reply("Removed track from queue in position " + std::to_string(position));
    } else if (subcommand.name == "shuffle") {
        if (!guild_audio_manager->shuffle(guild_id)) {
            event.reply("Queue is currently empty");
            return;
        }
        event.reply("Shuffled queue");

    } else if (subcommand.name == "loop_track") {
        bool loop_one = subcommand.get_value<bool>(0);
        if (!guild_audio_manager->loop_one(guild_id, loop_one)) {
            event.reply("Queue is currently empty");
            return;
        }
        event.reply("Queue looping set to: " + std::to_string(loop_one));
    } else if (subcommand.name == "loop_queue") {
        bool loop_all = subcommand.get_value<bool>(0);
        if (!guild_audio_manager->loop_all(guild_id, loop_all)) {
            event.reply("Queue is currently empty");
            return;
        }
        event.reply("Queue looping set to: " + std::to_string(loop_all));
    }
    else {
        event.reply("Unrecognised subcommand");
    }
}
