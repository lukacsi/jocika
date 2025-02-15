#ifndef ACHIEVEMENT_COMMAND_H
#define ACHIEVEMENT_COMMAND_H

#include "command.h"
#include "utils/achievement_manager.h"
#include "utils/permission_manager.h"
#include <memory>

class AchievementCommand : public Command {
public:
    AchievementCommand(std::shared_ptr<AchievementManager> _achievement_manager, std::shared_ptr<PermissionManager> _permission_manager)
        :Command("achievement", "Display achievements"), permission_manager(_permission_manager), achievement_manager(_achievement_manager){
        auto list = dpp::command_option(dpp::co_sub_command, "list", "List your achievements")
            .add_option(dpp::command_option(dpp::co_user, "user", "Specify user"));
        auto all = dpp::command_option(dpp::co_sub_command, "all", "List all achievments");
        auto add = dpp::command_option(dpp::co_sub_command, "add", "Add an achievement")
            .add_option(dpp::command_option(dpp::co_string, "name", "Name of the achievement", true))
            .add_option(dpp::command_option(dpp::co_string, "description", "Description of the achievement", true))
            .add_option(dpp::command_option(dpp::co_string, "image", "url with the image of the achievement", true));
        auto complete = dpp::command_option(dpp::co_sub_command, "complete", "Complete an achievement")
            .add_option(dpp::command_option(dpp::co_integer, "id", "Id of the achievement", true))
            .add_option(dpp::command_option(dpp::co_user, "user", "User to assign achievement to", true));
        auto view = dpp::command_option(dpp::co_sub_command, "view", "View an achievement")
            .add_option(dpp::command_option(dpp::co_integer, "id", "Id of achievement", true));
        add_option(list);
        add_option(all);
        add_option(add);
        add_option(complete);
        add_option(view);
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
private:
    std::shared_ptr<AchievementManager> achievement_manager;
    std::shared_ptr<PermissionManager> permission_manager;

};

#endif
