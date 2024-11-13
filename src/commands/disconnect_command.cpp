#include "commands/disconnect_command.h"
#include "utils/audio.h"

void DisconnectCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    audio_processor->disconnect_voice(event.command.guild_id);
    event.from->disconnect_voice(event.command.guild_id);
    event.reply("Disconnected from voice channel");
}
