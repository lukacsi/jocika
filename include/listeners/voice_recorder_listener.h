#ifndef VOICE_RECORDER_LISTENER_H
#define VOICE_RECORDER_LISTENER_H

#include "listener.h"
#include "commands/command_manager.h"
#include <memory>

class VoiceRecListener : public Listener {
public:
    VoiceRecListener(std::shared_ptr<CommandManager> cmdManager);
    void register_listener(dpp::cluster& bot) override;

private:
    std::shared_ptr<CommandManager> commandManager;
};



#endif //VOICE_RECORDER_LISTENER_H
