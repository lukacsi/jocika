#ifndef READY_LISTENER_H
#define READY_LISTENER_H

#include "commands/command_manager.h"
#include "listener.h"
#include <memory>

class ReadyListener : public Listener {
public:
    ReadyListener(std::shared_ptr<CommandManager> cmdManager/*, std::shared_ptr<Audio> _audio_processor*/);
    void register_listener(dpp::cluster& bot) override;

private:
    std::shared_ptr<CommandManager> commandManager;
    // std::shared_ptr<Audio> audio_processor;
};

#endif
