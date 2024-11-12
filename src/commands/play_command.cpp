#include "commands/play_command.h"
#include <string>

void PlayCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    std::string file_name;

    if (std::holds_alternative<std::string>(event.get_parameter("file"))) {
        file_name = std::get<std::string>(event.get_parameter("file"));
    }
            
    event.reply("");

}
