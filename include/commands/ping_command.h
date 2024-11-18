#ifndef PING_COMMAND_H
#define PING_COMMAND_H

#include "command.h"

class PingCommand : public Command {
public:
    PingCommand() : Command("ping", "Reply with Pong!") {}
    void execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) override;
};

#endif
