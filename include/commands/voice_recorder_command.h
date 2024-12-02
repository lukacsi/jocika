#ifndef VOICE_RECORDER_COMMAND_H
#define VOICE_RECORDER_COMMAND_H

#include "command.h"

class VoiceRecCommand : public Command {
public:
    VoiceRecCommand() : Command("record", "Record your own voice") {}
    void execute(const dpp::slashcommand_t &event, dpp::cluster &bot) override;
};



#endif //VOICE_RECORDER_COMMAND_H
