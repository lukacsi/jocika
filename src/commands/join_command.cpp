#include "commands/join_command.h"

void JoinCommand::execute(const dpp::slashcommand_t& event) {
    quiet_execute(event, false);
}

int JoinCommand::quiet_execute(const dpp::slashcommand_t& event, bool quiet) {
    auto ec = event.command;

    dpp::guild* g = dpp::find_guild(ec.guild_id);

    auto current_vc = event.from->get_voice(ec.guild_id);

    if (current_vc) {
        auto user_vc = g->voice_members.find(ec.get_issuing_user().id);

        if (user_vc != g->voice_members.end() && current_vc->channel_id == user_vc->second.channel_id) {
            if(!quiet)
                event.reply("Im aleady in your voice channel!");
            return JOIN_FAIL;
        } else {
            event.from->disconnect_voice(ec.guild_id);
        }
    }

    if (!g->connect_member_voice(ec.get_issuing_user().id)) {
        if(!quiet)
            event.reply("You dont seem to be in a voice channel.");
        return JOIN_FAIL;
    }

    if(!quiet)
        event.reply("Joined your channel.");

    return JOIN_SUCCESS;
}
