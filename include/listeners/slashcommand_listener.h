#ifndef SLASH_COMMAND_H
#define SLASH_COMMAND_H

#include "listener.h"
#include "commands/command_manager.h"
#include <memory>

class SlashCommandListener : public Listener {
public:
    SlashCommandListener(std::shared_ptr<CommandManager> cmdManager);
    void register_listener(dpp::cluster& bot) override;

private:
    std::shared_ptr<CommandManager> commandManager;
};

#endif
