//
// Created by Microsoft Köszi on 20/11/2024.
//

#ifndef VOICE_RECORDER_STOP_COMMAND_H
#define VOICE_RECORDER_STOP_COMMAND_H
#include "command.h"
#include "globals.h"


class VoiceRecStopCommand : public Command {
public:
    VoiceRecStopCommand() : Command("record_stop","Stops your voice recording.") {}
    void execute(const dpp::slashcommand_t &event, dpp::cluster &bot) override;
};



#endif //VOICE_RECORDER_STOP_COMMAND_H
