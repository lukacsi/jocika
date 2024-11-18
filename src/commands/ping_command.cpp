#include "commands/ping_command.h"

void PingCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    event.reply("Pong!");
}
