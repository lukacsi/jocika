#include "commands/skip_command.h"

void SkipCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    auto guild_id = event.command.guild_id;
    audio_processor->stop_audio(guild_id);
    guild_audio_manager->skip_track(guild_id);
    event.reply("Skipped current track!");
}
