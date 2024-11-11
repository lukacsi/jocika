#include "commands/command_manager.h"
#include "commands/join_command.h"
#include "commands/ping_command.h"
#include "commands/disconnect_command.h"
#include "listeners/listener_manager.h"
#include "listeners/ready_listener.h"
#include "listeners/slashcommand_listener.h"
#include <dpp/dpp.h>
#include <memory>


int main() {
    const char* token_env = std::getenv("DISCORD_BOT_TOKEN");
    if(!token_env) {
        std::cerr << "DISCORD_BOT_TOKEN environmental variable not set." << std::endl;
        return EXIT_FAILURE;
    }
    const std::string token = token_env;
    dpp::cluster bot(token);

    bot.on_log(dpp::utility::cout_logger());

    auto sharedCommandManager = std::make_shared<CommandManager>();
    sharedCommandManager->add_command(std::make_unique<PingCommand>());
    sharedCommandManager->add_command(std::make_unique<JoinCommand>());
    sharedCommandManager->add_command(std::make_unique<DisconnectCommand>());

    auto listenerManager = std::make_unique<ListenerManager>();
    listenerManager->add_listener(std::make_unique<ReadyListener>(sharedCommandManager));
    listenerManager->add_listener(std::make_unique<SlashCommandListener>(sharedCommandManager));

    listenerManager->register_listeners(bot);

    bot.start(dpp::st_wait);
    return EXIT_SUCCESS;
} 
