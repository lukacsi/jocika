#include "commands/ping_command.h"

void PingCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    event.reply("Pong!");
}
