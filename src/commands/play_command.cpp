#include "commands/play_command.h"
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <string>
#include <unistd.h>
#include <variant>

void PlayCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    auto guild_id = event.command.guild_id;
    std::string source;
    bool source_specified = false;
    if (std::holds_alternative<std::string>(event.get_parameter("file"))) {
        source = std::get<std::string>(event.get_parameter("file"));
        source_specified = true;
    }
    bool url = false;
    if(std::holds_alternative<std::string>(event.get_parameter("url"))) {
        source = std::get<std::string>(event.get_parameter("url"));
        source_specified = true;
        url = true;
    }

    dpp::guild* g = dpp::find_guild(guild_id);

    auto vc = audio_processor->get_voice_connection(guild_id);
    if (!vc) {
        audio_processor->join_voice(g, event.command.get_issuing_user().id);
        audio_processor->set_voice_connection(guild_id, event.from->get_voice(guild_id));
    }
    
    // pass tracks directry TODO
    if (source_specified) {
        if (url) {
            track_library->add_track(source, source, SourceType::Youtube);
        }
        auto tracks = guild_audio_manager->queue_track(guild_id, source);
        std::string reply = "Multiple tracks found:\n";
        if(tracks.size() > 1) {
            for (const auto& track : tracks) {
                reply += track->get_name() + '\n';
            }
            event.reply(reply);
            return;
        }
        event.reply("Queued track: " + tracks[0]->get_name());
    } else {
        guild_audio_manager->queue_all(guild_id);
        event.reply("Queued all tracks in library");
    }
}
