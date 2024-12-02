#include "commands/play_command.h"
#include <dpp/appcommand.h>
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <string>
#include <unistd.h>
#include <vector>

/*
std::optional<std::pair<bool, std::string>> interpret_subcommand(dpp::command_data_option subcommand) {
    std::string source;
    bool top;
    if(subcommand.options.size() == 0) {
        return std::nullopt;
    }
    else if (subcommand.options.size() == 1) {
        source = subcommand.get_value<std::string>(0);
        top = false;
        return std::pair(top, source);
    }
    for (int i = 0; i < 2; i++) {
        auto option = subcommand.options[i];
        if (option.name == "top") {
            top = subcommand.get_value<bool>(i);
        }
        else {
            source = subcommand.get_value<std::string>(i);
        }
    }
    return std::pair(top, source); 

}
*/

void PlayCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    dpp::command_interaction cmd_data = event.command.get_command_interaction();
    auto guild_id = event.command.guild_id;

    if (cmd_data.options.empty()) {
        event.reply("No action specified");
        return;
    }

    dpp::guild* g = dpp::find_guild(guild_id);

    auto vc = audio_processor->get_voice_connection(guild_id);
    if (!vc) {
        audio_processor->join_voice(g, event.command.get_issuing_user().id);
        audio_processor->set_voice_connection(guild_id, event.from->get_voice(guild_id));
    }
    
    auto subcommand = cmd_data.options[0];

    std::string source;
    bool top;
    
    if (subcommand.options.size() == 1) {
        source = subcommand.get_value<std::string>(0);
        top = false;
    } else if (subcommand.options.size() == 2) {
        for (int i = 0; i < 2; i++) {
            auto option = subcommand.options[i];
            if (option.name == "top") {
                top = subcommand.get_value<bool>(i);
            }
            else {
                source = subcommand.get_value<std::string>(i);
            }
        }
    }

    int i = max_tracks_reply;
    auto subcommand_name = subcommand.name;
    if (subcommand_name == "all") {
        guild_audio_manager->queue_all(guild_id);
        event.reply("Queued all tracks in library");
    }
    else if (subcommand_name == "from_library") {
        std::vector<std::shared_ptr<Track>> tracks; 
        tracks = guild_audio_manager->queue_track(guild_id, source, top);

        if(tracks.size() > 1) {
            std::string reply = "Multiple tracks found:\n";
            for (const auto& track : tracks) {
                if (i > 0) {
                    i--;
                    reply += track->get_name() + '\n';
                }
            }
            if (i == 0) {
                reply += "...\n";
            }
            event.reply(reply);
            return;
        }
        if (tracks.empty()) {
            event.reply("Track not found!");
            return;
        }
        event.reply("Queued track: " + tracks[0]->get_name());

    }
    else if (subcommand_name == "like") {
        auto tracks= guild_audio_manager->queue_track(guild_id, source);
        if (tracks.size() > 0) {
            std::string reply = "Queued " + std::to_string(tracks.size()) +  " track(s):\n"; 
            for (const auto& track : tracks) {
                auto name = track->get_name();
                guild_audio_manager->queue_track(guild_id, name, top);
                if (i > 0) {
                    i--;
                    reply += name + '\n';
                }
            }
            if (i == 0) {
                reply += "...\n";
            }
            event.reply(reply);
        } else {
            event.reply("No matching tracks were found!");
        }
    }
    else if (subcommand_name == "from_youtube") {
        event.thinking(true, [event, this, guild_id, source, top, &i](const dpp::confirmation_callback_t& callback) {
            event.edit_original_response(dpp::message("Downloading track data"));
            auto names = track_library->add_url_tracks(source);
            if (names.empty()) {
                event.edit_original_response(dpp::message("Failed to initialize track."));
                return;
            }

            std::string reply = "Queued " + std::to_string(names.size()) +  " track(s):\n"; 

            for (const auto& name : names) {
                guild_audio_manager->queue_track(guild_id, name, top);
                if (i > 0) {
                    i--;
                    reply += name + '\n';
                }
            }
            if (i == 0) {
                reply += "...\n";
            }
            event.edit_original_response(dpp::message(reply));
        });

    }
    else {
        event.reply("Unrecognised subcommand");
    }
}
