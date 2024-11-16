#include "listeners/ready_listener.h"
#include <memory>

ReadyListener::ReadyListener(std::shared_ptr<CommandManager> cmdMan, bool _register_commands)
    : commandManager(std::move(cmdMan)), register_commands(_register_commands) {
    //audio_processor = _audio_processor;
}

void ReadyListener::register_listener(dpp::cluster& bot) {
    bot.on_ready([this, &bot](const dpp::ready_t& event) {
        if (register_commands) {
            commandManager->register_commands(bot);
        }
        //audio_processor->recover_voice_connections(event.guilds);
    });
}
