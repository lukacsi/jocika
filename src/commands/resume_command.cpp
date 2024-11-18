#include "commands/resume_command.h"

void ResumeCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    auto ec = event.command;
    auto guild_id = ec.guild_id;
    //audio_processor->resume_audio(guild_id);
    guild_audio_manager->resume_track(guild_id);
    event.reply("Resumed track.");
}
