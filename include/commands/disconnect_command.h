#ifndef DISCONNECT_COMMAND_H
#define DISCONNECT_COMMAND_H

#include "command.h"

#define DISCONNECT_SUCCESS 2111
#define DISCONNECT_FAIL 2444

class DisconnectCommand: public Command {
public:
    DisconnectCommand() : Command("disconnect", "Disconnects form voice") {}
    void execute(const dpp::slashcommand_t& event) override;
    int quiet_execute(const dpp::slashcommand_t& event, bool quiet);
};

#endif
