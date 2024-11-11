#ifndef JOIN_COMMAND_H
#define JOIN_COMMAND_H

#include "command.h"

class JoinCommand : public Command {
public:
    JoinCommand() : Command("join", "Joins your voice channel.") {}
    void execute(const dpp::slashcommand_t& event) override;
};

#endif
