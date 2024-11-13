#ifndef PLAY_COMMAND_H
#define PLAY_COMMAND_H

#include "command.h"
#include "utils/audio.h"
#include <memory>

class Audio;

class PlayCommand : public Command {
public:
    PlayCommand(std::shared_ptr<Audio> audio) : Command("play", "Plays mp3 in voice channel."), audio(audio) {
        add_option(dpp::command_option(dpp::co_string, "file", "Specify the file name to play", false));
    }
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
private:
    std::shared_ptr<Audio> audio;
};

#endif
