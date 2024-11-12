#ifndef PLAY_COMMAND_H
#define PLAY_COMMAND_H

#include "command.h"

class PlayCommand : public Command {
public:
    PlayCommand() : Command("play", "Plays mp3 in voice channel.") {
        add_option(dpp::command_option(dpp::co_string, "file", "Specify the file name to play", false));
    }
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
};

#endif
