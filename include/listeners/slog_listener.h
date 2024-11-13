#ifndef SLOG_L_COMMAND_H
#define SLOG_L_COMMAND_H

#include "listener.h"
#include "commands/command_manager.h"
#include <memory>
#include <fstream>

class SLogListener : public Listener {
public:
    SLogListener(std::shared_ptr<CommandManager> cmdManager);
    void register_listener(dpp::cluster& bot) override;

private:
    std::shared_ptr<CommandManager> commandManager;
};

#endif
