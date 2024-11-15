#ifndef TRACK_LIBRARY_H
#define TRACK_LIBRARY_H

#include "utils/track.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>


class Track;

class TrackLibrary {
public:
    TrackLibrary();
    ~TrackLibrary();

    bool init_tracks(const std::string& media_dir);
    bool add_track(const std::string& name, const std::string& source, SourceType source_type);
    bool remove_track(const std::string& name);

    std::shared_ptr<Track> get_track(const std::string& name) const;
    std::vector<std::shared_ptr<Track>> get_all_matching_tracks(const std::string& name) const;
    std::vector<std::shared_ptr<Track>> get_all_tracks() const;


    std::vector<std::string> get_all_track_names() const;
    size_t get_length_ms(const std::string& name) const;

private:
    std::map<std::string, std::shared_ptr<Track>> tracks;
    mutable std::mutex tracks_mutex;
};

#endif
