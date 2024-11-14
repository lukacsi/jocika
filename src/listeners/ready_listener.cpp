#include "listeners/ready_listener.h"
#include <memory>

ReadyListener::ReadyListener(std::shared_ptr<CommandManager> cmdMan/*, std::shared_ptr<Audio> _audio_processor*/)
    : commandManager(std::move(cmdMan)) {
    //audio_processor = _audio_processor;
}

void ReadyListener::register_listener(dpp::cluster& bot) {
    bot.on_ready([this, &bot](const dpp::ready_t& event) {
        commandManager->register_commands(bot);
        //audio_processor->recover_voice_connections(event.guilds);
    });
}
