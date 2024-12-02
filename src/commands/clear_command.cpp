#include "commands/clear_command.h"

void ClearCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    auto guild_id = event.command.guild_id;
    guild_audio_manager->clear_queue(guild_id);
    event.reply("Queue cleared!");
}
