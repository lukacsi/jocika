#ifndef LIBRARY_COMMAND_H
#define LIBRARY_COMMAND_H

#include "command.h"
#include "utils/track_library.h"

class LibraryCommand : public Command {
public:
    LibraryCommand(std::shared_ptr<TrackLibrary> _track_library) 
    : Command("library", "Displays all tracks in library") {
        track_library = _track_library;
        add_option(dpp::command_option(dpp::co_string, "search", "Search file by loose name", false));
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
private:
    std::shared_ptr<TrackLibrary> track_library;
};

#endif
