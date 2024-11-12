#include "commands/join_command.h"
#include <dpp/cache.h>
#include <dpp/guild.h>

void JoinCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    auto ec = event.command;

    dpp::guild* g = dpp::find_guild(ec.guild_id);

    auto current_vc = event.from->get_voice(ec.guild_id);

    if (current_vc) {
        auto user_vc = g->voice_members.find(ec.get_issuing_user().id);

        if (user_vc != g->voice_members.end() && current_vc->channel_id == user_vc->second.channel_id) {
            event.reply("Im aleady in your voice channel!");
            return;
        } else {
            event.from->disconnect_voice(ec.guild_id);
        }
    }

    if (!g->connect_member_voice(ec.get_issuing_user().id)) {
        event.reply("You dont seem to be in a voice channel.");
        return;
    }
    event.reply("Joined your channel.");
}
