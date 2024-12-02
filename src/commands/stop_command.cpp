#include "commands/stop_command.h"

void StopCommand::execute(const dpp::slashcommand_t &event, dpp::cluster &bot) {
    auto guild_id = event.command.guild_id;
    //audio_processor->stop_audio(guild_id);
    guild_audio_manager->clear_queue(guild_id);
    guild_audio_manager->skip_track(guild_id);
    event.reply("Stopped playback and cleared queue!");
}
