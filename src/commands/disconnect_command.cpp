#include "commands/disconnect_command.h"

void DisconnectCommand::execute(const dpp::slashcommand_t& event) {
    auto ec = event.command;

    dpp::guild* g = dpp::find_guild(ec.guild_id);

    auto current_vc = event.from->get_voice(ec.guild_id);

    if (current_vc) {
        event.from->disconnect_voice(ec.guild_id);
        event.reply("Disconnected from voice channel!");
        return;
    }

    event.reply("Not in a voice channel!");
}
