#include "commands/library_command.h"

void LibraryCommand::execute(const dpp::slashcommand_t& event, dpp::cluster& bot) {
    std::string name;
    bool search_specified = false;
    if (std::holds_alternative<std::string>(event.get_parameter("search"))) {
        name = std::get<std::string>(event.get_parameter("search"));
        search_specified = true;
    }

    std::vector<std::shared_ptr<Track>> tracks;
    if (search_specified) {
        tracks = track_library->get_all_matching_tracks(name);
    } else {
        tracks = track_library->get_all_tracks();
    }
    
    if (tracks.empty()) {
        event.reply("No tracks found!");
        return;
    }

    std::string reply = search_specified ? "Tracks found:\n" : "Library contents:\n";
    
    for (const auto& it : tracks) {
        reply += it->toString() + '\n';
    }

    event.reply(reply);
}
