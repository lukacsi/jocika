#ifndef VOICE_COMMAND_H
#define VOICE_COMMAND_H

#include "command.h"
#include "utils/guild_audio_manager.h"
#include "utils/audio.h"
#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <memory>
#include <string>

class VoiceCommand : public Command {
public:
    VoiceCommand(std::string name, std::string description,
                 std::shared_ptr<Audio> _audio_processor,
                 std::shared_ptr<GuildAudioManager> _guild_audio_manager) 
        : Command(name, description),
          audio_processor(_audio_processor),
          guild_audio_manager(_guild_audio_manager) {}
    virtual ~VoiceCommand() = default;
    virtual void execute(const dpp::slashcommand_t& event, const dpp::cluster&bot) = 0;

protected:
    std::shared_ptr<Audio> audio_processor;
    std::shared_ptr<GuildAudioManager> guild_audio_manager;
};

#endif
