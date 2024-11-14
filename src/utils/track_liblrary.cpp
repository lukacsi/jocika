#include "utils/track_library.h"
#include <filesystem>
#include <iostream>
#include <mutex>

TrackLibrary::TrackLibrary() {};

bool TrackLibrary::load_tracks(const std::string& media_dir) {
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

            std::shared_ptr<Track> track = std::make_shared<Track>(track_name, file_path);
            if (track->load()) {
                tracks[track_name] = track;
                any_loaded = true;
                std::cout << "[TrackLibrary] Audio file loaded: " << track_name << std::endl;
            }
            else {
                std::cerr << "[TrackLibrary] Failed to load track: " << track_name << std::endl;
            }
        }
    }

    if (!any_loaded) {
        std::cerr << "[TrackLibrary] No tracks loaded from directory: " << media_dir << std::endl;
    }
    else {
        std::cout << "[TrackLibrary] Successfully loaded tracks from directory: " << media_dir << std::endl;
    }

    return any_loaded;
}

bool TrackLibrary::add_track(const std::string& name, const std::string& file_path) {
    std::lock_guard<std::mutex> lock(tracks_mutex);

    if (tracks.find(name) != tracks.end()) {
        std::cerr << "[TrackLibrary] Track with name '" << name << "' already exists." << std::endl;
        return false;
    }

    std::shared_ptr<Track> track = std::make_shared<Track>(name, file_path);
    if (track->load()) {
        tracks[name] = track;
        std::cout << "[TrackLibrary] Added and loaded track: " << name << std::endl;
        return true;
    }
    else {
        std::cerr << "[TrackLibrary] Failed to load track: " << name << std::endl;
        return false;
    }
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

std::shared_ptr<Track> TrackLibrary::get_track(const std::string& name) const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    auto it = tracks.find(name);
    if (it != tracks.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> TrackLibrary::get_all_track_names() const {
    std::lock_guard<std::mutex> lock(tracks_mutex);
    std::vector<std::string> track_names;
    for (const auto& [name, _] : tracks) {
        track_names.push_back(name);
    }
    return track_names;
}

size_t TrackLibrary::get_length_ms(const std::string& name) const {
    return get_track(name)->get_length();
}
