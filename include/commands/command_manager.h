#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "command.h"
#include <vector>
#include <memory>

class CommandManager {
public:
    void add_command(std::unique_ptr<Command> cmd);
    void register_commands(dpp::cluster& bot);
    void execute_command(const dpp::slashcommand_t& event);

private:
    std::vector<std::unique_ptr<Command>> commands;
};

#endif
