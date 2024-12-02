#include "listeners/voice_recorder_listener.h"

#include <commands/command_manager.h>

VoiceRecListener::VoiceRecListener(std::shared_ptr<CommandManager> cmdManager)
  : commandManager(cmdManager) {}


void VoiceRecListener::register_listener(dpp::cluster &bot) {

  bot.on_voice_receive([&bot](const dpp::voice_receive_t & event) {
    if(Globals::get_instance().is_vr_on()) {
      std::cout <<"Recording voice...\n";
      fwrite((char*)event.audio,1,event.audio_size,Globals::get_instance().vc_file());
    }
  });
}

