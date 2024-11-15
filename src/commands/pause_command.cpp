#include "commands/pause_command.h"

void PauseCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    auto ec = event.command;
    auto guild_id = ec.guild_id;
    //audio_processor->pause_audio(guild_id);
    guild_audio_manager->pause_track(guild_id);
    event.reply("Paused currect track.");
}
