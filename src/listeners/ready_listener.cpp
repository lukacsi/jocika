#include "listeners/ready_listener.h"

ReadyListener::ReadyListener(std::shared_ptr<CommandManager> cmdMan)
    : commandManager(std::move(cmdMan)) {}

void ReadyListener::register_listener(dpp::cluster& bot) {
    bot.on_ready([this, &bot](const dpp::ready_t& event) {
        commandManager->register_commands(bot);
    });
}
