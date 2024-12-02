//
// Created by Microsoft Köszi on 20/11/2024.
//

#include "commands/voice_recorder_command.h"

void VoiceRecCommand::execute(const dpp::slashcommand_t &event, dpp::cluster &bot) {
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        event.reply(dpp::ir_deferred_channel_message_with_source, "Processing...");
        event.edit_response("Beginning voice recording preparations!");
        dpp::guild* gld = dpp::find_guild(event.command.guild_id);
        Globals::get_instance().vc_file();
        Globals::get_instance().vr_enable();

        if(!gld->connect_member_voice(event.command.get_issuing_user().id)) {
            event.reply("You are not in a voice channel");
            return;
        }

        event.edit_response("Joining voice channel, begin recording.");

    });
}
