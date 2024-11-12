#ifndef DISCONNECT_COMMAND_H
#define DISCONNECT_COMMAND_H

#include "command.h"

class DisconnectCommand: public Command {
public:
    DisconnectCommand() : Command("disconnect", "Disconnects form voice") {}
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
};

#endif
