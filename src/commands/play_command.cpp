#include "commands/play_command.h"
#include "utils/audio.h"
#include "utils/audio_utils.h"
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <string>
#include <unistd.h>

void PlayCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    auto guild_id = event.command.guild_id;
    std::string file_name;
    bool file_specified = false;
    if (std::holds_alternative<std::string>(event.get_parameter("file"))) {
        file_name = std::get<std::string>(event.get_parameter("file"));
        file_specified = true;
    }

    dpp::guild* g = dpp::find_guild(guild_id);

    //audio_processor->join_voice(g, event.command.get_issuing_user().id);
    //audio_processor->set_voice_connection(guild_id, event.from->get_voice(guild_id));
    if (file_specified) {
        guild_audio_manager->queue_track(guild_id, file_name);
        event.reply("Queued track: " + file_name);
    } else {
        guild_audio_manager->queue_all(guild_id);
        event.reply("Queued all tracks in library");
    }
}
