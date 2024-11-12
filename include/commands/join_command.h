#ifndef JOIN_COMMAND_H
#define JOIN_COMMAND_H

#include "command.h"

#define JOIN_SUCCESS 1111 
#define JOIN_FAIL 1444

class JoinCommand : public Command {
public:
    JoinCommand() : Command("join", "Joins your voice channel.") {}
    void execute(const dpp::slashcommand_t& event) override;
    int quiet_execute(const dpp::slashcommand_t & event, bool quiet);
};

#endif
