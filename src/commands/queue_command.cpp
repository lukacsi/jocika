#include "commands/queue_command.h"

void QueueCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    if (!guild_audio_manager) {
        std::cerr << "[QueueCommand] guild_audio_manager is not initialized." << std::endl;
        event.reply("Internal error: Audio manager not initialized.");
        return;
    }

    auto guild_id = event.command.guild_id;
    std::vector<std::string> tracks = guild_audio_manager->get_queued_tracks(guild_id);

    if (tracks.empty()) {
        event.reply("Queue is currently empty.");
        return;
    }

    std::string reply_message = "Current Queue:\n";
    for (size_t i = 0; i < tracks.size(); ++i) {
        reply_message += std::to_string(i + 1) + ". " + tracks[i] + "\n";
    }
    
    event.reply(reply_message);

}
