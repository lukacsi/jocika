#include "listeners/slashcommand_listener.h"
#include <memory>

SlashCommandListener::SlashCommandListener(std::shared_ptr<CommandManager> cmdMan)
    : commandManager(cmdMan) {}

void SlashCommandListener::register_listener(dpp::cluster& bot) {
    bot.on_slashcommand([this, &bot](const dpp::slashcommand_t& event){
        commandManager->execute_command(event, bot);
    });
}
