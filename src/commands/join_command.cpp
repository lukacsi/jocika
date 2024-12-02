#include "commands/join_command.h"

void JoinCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    auto ec = event.command;
    auto guild_id = ec.guild_id;

    dpp::guild* g = dpp::find_guild(guild_id);

    auto current_vc = event.from->get_voice(guild_id);

    if (current_vc) {
        auto user_vc = g->voice_members.find(ec.get_issuing_user().id);

        if (user_vc != g->voice_members.end() && current_vc->channel_id == user_vc->second.channel_id) {
            event.reply("Im aleady in your voice channel!");
            return;
        } else {
            event.from->disconnect_voice(guild_id);
        }
    }

    audio_processor->join_voice(g, event.command.get_issuing_user().id);
    audio_processor->set_voice_connection(guild_id, event.from->get_voice(guild_id));
    event.reply("Joined your channel.");
}
