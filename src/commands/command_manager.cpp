#include "commands/command_manager.h"

void CommandManager::add_command(std::unique_ptr<Command> cmd) {
    commands.emplace_back(std::move(cmd));
}

void CommandManager::register_commands(dpp::cluster& bot) {
    for (auto& cmd : commands) {
        cmd->register_command(bot);
    }
}

void CommandManager::execute_command(const dpp::slashcommand_t& event) {
    for (auto& cmd : commands) {
        if (event.command.get_command_name() == cmd->get_name()) {
            cmd->execute(event);
            break;
        }
    }
}
