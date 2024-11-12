#include "commands/disconnect_command.h"

void DisconnectCommand::execute(const dpp::slashcommand_t& event) {
    quiet_execute(event, false);
}

int DisconnectCommand::quiet_execute(const dpp::slashcommand_t& event, bool quiet) {
    auto ec = event.command;

    dpp::guild* g = dpp::find_guild(ec.guild_id);

    auto current_vc = event.from->get_voice(ec.guild_id);

    if (current_vc) {
        event.from->disconnect_voice(ec.guild_id);
        if (!quiet)
            event.reply("Disconnected from voice channel!");
        return DISCONNECT_SUCCESS;
    }

    if (!quiet)
        event.reply("Not in a voice channel!");
    return DISCONNECT_FAIL;
}
