#include "commands/play_command.h"
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <string>
#include <unistd.h>
#include <variant>

void PlayCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {
    dpp::command_interaction cmd_data = event.command.get_command_interaction();
    auto guild_id = event.command.guild_id;
    std::string source;

    dpp::guild* g = dpp::find_guild(guild_id);
    
    bool source_specified = false;
    bool url = false;
    bool like = false;
    bool top = false;

    if (cmd_data.options.size() > 0) {
        for (const auto& option : cmd_data.options) {
            if (option.name == "top" && std::holds_alternative<bool>(option.value)) {
                top = std::get<bool>(option.value);
            } else if (option.name == "file" && std::holds_alternative<std::string>(option.value)) {
                source = std::get<std::string>(option.value);
            } else if (option.name == "url" && std::holds_alternative<std::string>(option.value)) {
                source = std::get<std::string>(option.value);
                url = true;
            } else if (option.name == "like" && std::holds_alternative<std::string>(option.value)) {
                source = std::get<std::string>(option.value);
                like = true;
            }
        }
    }


    auto vc = audio_processor->get_voice_connection(guild_id);
    if (!vc) {
        audio_processor->join_voice(g, event.command.get_issuing_user().id);
        audio_processor->set_voice_connection(guild_id, event.from->get_voice(guild_id));
    }

    if (cmd_data.options.size() > 2 && !(top && cmd_data.options.size() == 2)) {
        event.reply("Invalid number of parameters!");
        return;
    }

    if (cmd_data.options.empty()) {
        //seg fault
        guild_audio_manager->queue_all(guild_id);
        event.reply("Queued all tracks in library");
    }
    

    //std::cout << (top?"true":"false") << std::endl;
    if (url) {
        event.thinking(true, [event, this, guild_id, source, top](const dpp::confirmation_callback_t& callback) {
            auto name = track_library->add_track("", source, SourceType::Youtube);
            if (name == "") {
                event.edit_original_response(dpp::message("Failed to initialize track."));
                return;
            }
            if (top) {
                guild_audio_manager->queue_track_top(guild_id, name);
            } else {
                guild_audio_manager->queue_track(guild_id, name);
            }
            event.edit_original_response(dpp::message("Queued track: " + name));
        });
    } else if(like) {
        auto tracks = guild_audio_manager->queue_track(guild_id, source);
        if (tracks.size() > 0) {
            std::string reply = "Queued track(s):";
            for (const auto& track : tracks) {
                auto name = track->get_name();
                if (top) {
                    guild_audio_manager->queue_track_top(guild_id, name);
                } else {
                    guild_audio_manager->queue_track(guild_id, name);
                }
                reply += name + '\n';
            }
            event.reply(reply);
        } else {
            event.reply("No matching tracks were found!");
        }
    } else {
        std::vector<std::shared_ptr<Track>> tracks; 
        if (top) {
             tracks = guild_audio_manager->queue_track_top(guild_id, source);           
        } else {
            tracks = guild_audio_manager->queue_track(guild_id, source);

        }
        std::string reply = "Multiple tracks found:\n";
        if(tracks.size() > 1) {
            for (const auto& track : tracks) {
                reply += track->get_name() + '\n';
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
}
