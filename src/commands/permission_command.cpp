#include "commands/permission_command.h"

void PermissionCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    auto cmd_data = event.command.get_command_interaction();
    auto subcommand = cmd_data.options[0];

    if (subcommand.name == "set") {
        if (!permission_manager->check_permission(event.command.usr.id, ADMIN_PERMISSION)) {
            event.reply("You dont have permission to do this!");
            return;
        }

        int permission_level;
        uint64_t user_id;

        for (int i = 0; i < 2; i++) {
            auto option = subcommand.options[i];
            if (option.name == "user") {
                user_id =  subcommand.get_value<dpp::snowflake>(i);
            }
            else if (option.name == "permission") {
                permission_level = subcommand.get_value<int>(i);
            }
        }

        permission_manager->set_permission(user_id, permission_level);
        event.reply("Permision set!");
    }
}
