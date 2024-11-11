#include "commands/play_command.h"
#include <dpp/appcommand.h>
#include <string>

void PlayCommand::execute(const dpp::slashcommand_t& event) {
    std::string file_name;

    if (std::holds_alternative<std::string>(event.get_parameter("file"))) {
        file_name = std::get<std::string>(event.get_parameter("file"));
    }
            
    event.reply("");
}
