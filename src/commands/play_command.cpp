#include "commands/play_command.h"
#include "utils/audio_utils.h"
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <string>
#include <unistd.h>

void PlayCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    std::string file_name;
    bool file_specified = false;
    if (std::holds_alternative<std::string>(event.get_parameter("file"))) {
        file_name = std::get<std::string>(event.get_parameter("file"));
        file_specified = true;
    }

    if (AudioUtils::join_voice_channel(event)) {

    }
    dpp::voiceconn* vc = event.from->get_voice(event.command.guild_id);
    audio->play_track(event.command.guild_id, file_name, vc);
    event.reply("Queued track: " + file_name);

}
