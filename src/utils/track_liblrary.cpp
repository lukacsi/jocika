#include "utils/track_library.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <mpg123.h>
#include <mutex>
#include <vector>

TrackLibrary::TrackLibrary() {}
TrackLibrary::~TrackLibrary() {}

std::string to_lower(const std::string& str) {
    std::string lower_str;
    lower_str.reserve(str.size());

    for (char c : str) {
        lower_str += std::tolower(static_cast<unsigned char>(c));
    }

    return lower_str;
}

bool TrackLibrary::init_tracks(const std::string& media_dir) {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    bool any_loaded = false;

    for (const auto& entry : std::filesystem::directory_iterator(media_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            std::string track_name = entry.path().stem().string();
            std::string file_path = entry.path().string();

            if (tracks.find(track_name) != tracks.end()) {
                std::cerr << "[TrackLibrary] Duplicate track name detected: " << track_name << ". Skipping." << std::endl;
                continue;
            }

            std::shared_ptr<Track> track = std::make_shared<Track>(track_name, file_path, SourceType::Local);
            if (track->load()) {
                track->unload();
                tracks[track_name] = track;
                any_loaded = true;
                std::cout << "[TrackLibrary] Audio file initialized: " << track_name << std::endl;
            }
            else {
                std::cerr << "[TrackLibrary] Failed to initialize track: " << track_name << std::endl;
            }
        }
    }

    if (!any_loaded) {
        std::cerr << "[TrackLibrary] No tracks initialized from directory: " << media_dir << std::endl;
    }
    else {
        std::cout << "[TrackLibrary] Successfully initialized tracks from directory: " << media_dir << std::endl;
    }

    return any_loaded;
}

bool TrackLibrary::add_track(const std::string& name, const std::string& source, SourceType source_type) {
    std::lock_guard<std::mutex> lock(tracks_mutex);

    if (tracks.find(name) != tracks.end()) {
        std::cerr << "[TrackLibrary] Track with name '" << name << "' already exists." << std::endl;
        return false;
    }

    std::shared_ptr<Track> new_track = std::make_shared<Track>(name, source, source_type);
    tracks[name] = new_track;
    std::cout << "[TrackLibrary] Added track: " << name << " with source_type: " << ((source_type==SourceType::Youtube)?"Youtube":"Local") << std::endl;
    return true;
    /*std::shared_ptr<Track> track = std::make_shared<Track>(name, file_path);
    if (track->load()) {
        track->unload();
        tracks[name] = track;
        std::cout << "[TrackLibrary] Added track: " << name << std::endl;
        return true;
    }
    else {
        std::cerr << "[TrackLibrary] Failed to load track: " << name << std::endl;
        return false;
    }*/
}

bool TrackLibrary::remove_track(const std::string& name) {
    std::lock_guard<std::mutex> lock(tracks_mutex);

    auto it = tracks.find(name);
    if (it != tracks.end()) {
        tracks.erase(it);
        std::cout << "[TrackLibrary] Removed track: " << name << std::endl;
        return true;
    }
    else {
        std::cerr << "[TrackLibrary] Track not found: " << name << std::endl;
        return false;
    }
}

std::vector<std::string> TrackLibrary::get_all_track_names() const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    std::vector<std::string> track_names;
    for (const auto& [name, _] : tracks) {
        track_names.push_back(name);
    }
    return track_names;
}


std::shared_ptr<Track> TrackLibrary::get_track(const std::string& name) const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    auto it = tracks.find(name);
    if (it != tracks.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Track>> TrackLibrary::get_all_matching_tracks(const std::string& name) const {
    std::vector<std::shared_ptr<Track>> matching_tracks;
    std::vector<std::string> names;

    {
        std::lock_guard<std::mutex> lock(tracks_mutex);
        names.reserve(tracks.size());
        for (const auto& [track_name, _] : tracks) {
            names.push_back(track_name);
        }
    }

    std::string lower_name = to_lower(name);

    for (const auto& o_name : names) {
        std::string lower_o_name = to_lower(o_name);
        if (lower_o_name.find(lower_name) != std::string::npos) {
            std::lock_guard<std::mutex> lock(tracks_mutex);
            auto it = tracks.find(o_name);
            if (it != tracks.end()) {
                matching_tracks.push_back(it->second);
            }
        }
    }

    return matching_tracks;
}

std::vector<std::shared_ptr<Track>> TrackLibrary::get_all_tracks() const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    std::vector<std::shared_ptr<Track>> s_tracks;
    for (const auto& [name, track] : tracks) {
        s_tracks.push_back(track);
    }
    return s_tracks;
}


size_t TrackLibrary::get_length_ms(const std::string& name) const {
    return get_track(name)->get_length();
}

