#include "commands/achievement_command.h"
#include <cstdint>
#include <memory>
#include <string>

void AchievementCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    auto cmd_data = event.command.get_command_interaction();

    auto guild_id = event.command.guild_id;
    auto subcommand = cmd_data.options[0];

    if (subcommand.name == "list" || subcommand.name == "all") {
        bool is_list_all = (subcommand.name == "all");
        std::vector<std::shared_ptr<Achievement>> achievements;

        if (is_list_all) {
            achievements = achievement_manager->get_all_achievements();
        } else {
            uint64_t user_id = event.command.usr.id;
            if (!subcommand.options.empty()) {
                user_id = subcommand.get_value<dpp::snowflake>(0);
            }
            achievements = achievement_manager->get_user_achievements(user_id);
        }

        if (achievements.empty()) {
            event.reply(dpp::message(is_list_all ? "There are no achievements." : "You have no achievements yet."));
            return;
        }
        std::string reply = "Achievements\n";
        for (auto const& ach : achievements) {
            reply += "Name:" + ach->name + " ID:" + std::to_string(ach->id) + '\n';
        }

        event.reply(reply);
    } else if (subcommand.name == "add") {
        std::string name, description, image;
        for (int i = 0; i < 3; i++) {
            auto option = subcommand.options[i];
            if (option.name == "name") {
                name = subcommand.get_value<std::string>(i);
            } else if (option.name == "description") {
                description = subcommand.get_value<std::string>(i);
            } else if (option.name == "image") {
                image = subcommand.get_value<std::string>(i);
            }
        }
        auto achievement = std::make_shared<Achievement>(0, name, description, image);
        achievement_manager->add_achievement(achievement);
        if (!achievement) {
            event.reply("Failed to add achievement!");
            return;
        }
        event.reply("Added achievement: " + achievement->name + " " + std::to_string(achievement->id) + "\n"
                    + achievement->description + " " + achievement->image_url);
    } else if (subcommand.name == "complete") {
        uint64_t ach_id, user_id;
        
        for (int i = 0; i < 2; i++) {
            auto option = subcommand.options[i];
            if (option.name == "user") {
                user_id = subcommand.get_value<dpp::snowflake>(i);
            } else if (option.name == "id") {
                ach_id = subcommand.get_value<long>(i);
            }
        }
        if (!achievement_manager->assign_achievement(user_id, ach_id)) {
            event.reply("Failed to assign achivement");
            return;
        }
        event.reply("Assigned achivement to user!");

    } else if (subcommand.name == "view") {
        uint64_t id = subcommand.get_value<long>(0);
        auto achievement = achievement_manager->get_achievement(id);
        if (!achievement) {
            event.reply("Achivement not found!");
            return;
        }

        dpp::embed embed = dpp::embed()
            .set_title(achievement->name)
            .set_description(achievement->description)
            .set_image(achievement->image_url)
            .add_field(
                "Id",
                std::to_string(achievement->id),
                true
            )
            .add_field(
                "Rarity",
                std::to_string(achievement_manager->get_completion_stats(achievement->id)),
                true
            );
        dpp::message msg(event.command.channel_id, embed);
        event.reply(msg);
    } else {
        event.reply("Invalid subcommand!");
    }
}
