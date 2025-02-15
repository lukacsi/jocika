#ifndef PERMISSION_COMMAND_H
#define PERMISSION_COMMAND_H

#include "command.h"
#include "utils/permission_manager.h"
#include <memory>

class PermissionCommand : public Command {
public:
    PermissionCommand(std::shared_ptr<PermissionManager> _permission_manager)
    : Command("permission", "Set user permissions."), permission_manager(_permission_manager) {
        auto set = dpp::command_option(dpp::co_sub_command, "set", "Set an users permission")
            .add_option(dpp::command_option(dpp::co_user, "user", "User to assign permission to", true))
            .add_option(dpp::command_option(dpp::co_integer, "permission", "The level of permission to set", true)
                        .add_choice(dpp::command_option_choice("basic", 6001))
                        .add_choice(dpp::command_option_choice("regular", 6002))
                        .add_choice(dpp::command_option_choice("vip", 6003))
                        .add_choice(dpp::command_option_choice("admin", 6004)));
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;

private:
    std::shared_ptr<PermissionManager> permission_manager;
};

#endif
