#ifndef SLOG_COMMAND_H
#define SLOG_COMMAND_H

#include "command.h"

class SLogCommand : public Command {
public:
    SLogCommand() : Command("slog", "Start logging messages!") {
        add_option(dpp::command_option(dpp::co_user,
            "target",
            "name the target",
            true));
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
};

#endif
