#include "commands/achievement_command.h"

void AchievementCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    auto cmd_data = event.command.get_command_interaction();

    auto guild_id = event.command.guild_id;
    auto subcommand = cmd_data.options[0];

    if (subcommand.name == "list") {
    } else if (subcommand.name == "all") {
    } else if (subcommand.name == "add") {
    } else if (subcommand.name == "complete") {
    } else {
    }
}
