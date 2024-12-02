//
// Created by Microsoft Köszi on 20/11/2024.
//

#include "commands/voice_recorder_stop_command.h"

void VoiceRecStopCommand::execute(const dpp::slashcommand_t &event, dpp::cluster &bot) {
    event.from->disconnect_voice(event.command.guild_id);
    Globals::get_instance().vc_file_close();
    Globals::get_instance().vr_disable();

    event.reply("Stopped voice recording!");
}
