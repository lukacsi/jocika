#include "utils/audio_utils.h"

bool AudioUtils::join_voice_channel(const dpp::slashcommand_t& event) {
    auto ec = event.command;

    dpp::guild* g = dpp::find_guild(ec.guild_id);

    auto current_vc = event.from->get_voice(ec.guild_id);

    if (current_vc) {
        auto user_vc = g->voice_members.find(ec.get_issuing_user().id);

        if (user_vc != g->voice_members.end() && current_vc->channel_id == user_vc->second.channel_id) {
            return true;
        } else {
            event.from->disconnect_voice(ec.guild_id);
        }
    }

    if (!g->connect_member_voice(ec.get_issuing_user().id)) {
        return false;
    }
    return true;
}
