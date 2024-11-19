#include "commands/command_manager.h"

void CommandManager::add_command(std::unique_ptr<Command> cmd) {
    commands.emplace_back(std::move(cmd));
}

void CommandManager::register_commands(dpp::cluster& bot) {
    std::vector<dpp::slashcommand> slash_commands;
    for (auto& cmd : commands) {
        slash_commands.push_back(cmd->get_command(bot));
    }
    bot.global_bulk_command_create(slash_commands, [this](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            std::cerr << "Failed to register commands " << callback.get_error().message << std::endl;
        } else {
            std::cout << "Succesfully registered commands" << std::endl;
        }
    });
}

void CommandManager::execute_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    for (auto& cmd : commands) {
        if (event.command.get_command_name() == cmd->get_name()) {
            cmd->execute(event, bot);
            break;
        }
    }
}
