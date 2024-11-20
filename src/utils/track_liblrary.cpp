#include "utils/track_library.h"
#include <filesystem>
#include <memory>
#include <mpg123.h>
#include <mutex>
#include "globals.h"
#include <memory>
#include <array>
#include <mpg123.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
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

std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) start++;
    
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    
    return std::string(start, end + 1);
}

std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string select_best_audio_format(const std::vector<std::string>& formats) {
    std::vector<std::string> preferred_formats = {"251", "250", "249", "140"};

    for (const auto& fmt_code : preferred_formats) {
        for (const auto& fmt : formats) {
            if (fmt == fmt_code) {
                return fmt;
            }
        }
    }

    return "";
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
            if (track->init()) {
                tracks[track_name] = track;
                any_loaded = true;
                //std::cout << "[TrackLibrary] Audio file initialized: " << track_name << std::endl;
            }
            else {
                std::cerr << "[TrackLibrary] Failed to initialize track: " << track_name << std::endl;
            }
        }
    }

    if (!any_loaded) {
        std::cout << "[TrackLibrary] No tracks initialized from directory: " << media_dir << std::endl;
    }
    else {
        std::cout << "[TrackLibrary] Successfully initialized tracks from directory: " << media_dir << std::endl;
    }

    return any_loaded;
}

std::vector<std::string> TrackLibrary::add_url_tracks(const std::string& url) {
    std::vector<std::string> track_names;

    std::array<char, 128> buffer;
    std::string data;
    std::string command = "yt-dlp --cookies '" + cookies_path + "' --get-title --get-id --get-duration -F '" + url + "'";
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        std::cerr << "[Track] popen() failed!" << std::endl;
        return {};
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        data += buffer.data();
    }

    if (data.empty()) {
        return {};
    }

    std::vector<std::string> data_lines;
    std::istringstream stream(data);
    std::string line;
    while (std::getline(stream, line)) {
        data_lines.push_back(line);
    }

    struct TrackInfo {
        std::vector<std::string> format_lines;
        std::string track_title;
        std::string video_id;
        std::string duration;
    };

    std::vector<TrackInfo> new_tracks;
    TrackInfo current_track;
    std::vector<std::string> formats;
    bool in_formats = false;


    std::regex format_line_regex(R"(^\s*(?:\d+|sb\d+)\s+)");

    size_t idx = data_lines.size();
    while (idx > 0) {
        idx--;

        line = trim(data_lines[idx]);
        if (line.find("WARNING") != std::string::npos) {
            continue;
        }

        if (line.find("ERROR") != std::string::npos) {
            std::cerr << "[TrackLibrary] yt-dlp error occurred" << std::endl;
            return {};
        }

        if (!in_formats) {
            if (idx + 4 < 0) {
                std::cerr << "[TrackLibrary] Insufficient lines for track metadata." << std::endl;
                break;
            }

            std::string duration = line;
            idx--;

            std::string video_id = trim(data_lines[idx]);
            idx--;

            std::string track_title = trim(data_lines[idx]);
            current_track.track_title = track_title;
            current_track.video_id = video_id;
            current_track.duration = duration;

            in_formats = true;
            continue;
        }

        if (line.find("ID  EXT") != std::string::npos) {
            current_track.format_lines = formats;
            formats.clear();
            new_tracks.push_back(current_track);
            in_formats = false; 
            continue;
        }

        std::string potential_format_line = line;
        if (std::regex_search(potential_format_line, format_line_regex)) {
            std::vector<std::string> tokens = split(line);
            if (!tokens.empty()) {
                formats.push_back(tokens[0]);
            }
        }
    }

    std::reverse(new_tracks.begin(), new_tracks.end());

    for (const auto& track : new_tracks) {
        {
            std::lock_guard<std::mutex> lock(tracks_mutex);
            if (tracks.find(track.track_title) != tracks.end()) {
                std::cout << "[TrackLibrary] Track with name '" << track.track_title << "' already exists." << std::endl;
                continue;
            }
        }
        std::vector<std::string> formats;
        std::regex data_line_regex(R"(^\s*(\d+)\s+)");
        for (const auto& fmt_line : track.format_lines) {
            std::smatch match;
            if (std::regex_search(fmt_line, match, data_line_regex)) {
                formats.push_back(match[1]);
            }
        }

        std::string best_format = select_best_audio_format(track.format_lines);
        if (best_format == "") {
            std::cerr << "No suitable format found for track: " << track.track_title << std::endl;
            continue;;
        }

        std::vector<size_t> time_parts;
        std::stringstream tsream(track.duration);
        std::string part;
        size_t track_length = 0;

        while (std::getline(tsream, part, ':')) {
            time_parts.push_back(std::stoi(part));
        }
        if (time_parts.size() == 1) {
            track_length = time_parts[0];
        } else if (time_parts.size() == 2) {
            track_length = time_parts[0] * 60 + time_parts[1];
        } else if (time_parts.size() == 3) {
            track_length = time_parts[0] * 3600 + time_parts[1] * 60 + time_parts[2];
        } else {
            std::cerr << "Unexpected duration format: " << track.duration << " for tack:" << track.track_title << std::endl;
            continue;
        }
        track_length += 1;

        std::cout << "[TrackLibrary] Initialized YouTube track '" << track.track_title << "' duration " << track_length << " best format " << best_format << std::endl;

        {
            std::lock_guard<std::mutex> lock(tracks_mutex);
            std::shared_ptr<Track> new_track = std::make_shared<Track>(track.track_title, track.video_id, SourceType::Youtube, best_format, track_length);
            tracks[track.track_title] = new_track;
        }

        track_names.push_back(track.track_title);
    }

    return track_names;
}

/*
std::string TrackLibrary::add_track(const std::string& name, const std::string& source, SourceType source_type) {
    std::lock_guard<std::mutex> lock(tracks_mutex);

    std::shared_ptr<Track> new_track = std::make_shared<Track>(name, source, source_type);
    if (!new_track->init()) {
        std::cerr << "[TrackLibrary] Track failed to initialize." << std::endl;
        return "";
    }

    auto new_name = new_track->get_name();
    if (tracks.find(new_name) != tracks.end()) {
        std::cerr << "[TrackLibrary] Track with name '" << name << "' already exists." << std::endl;
        return new_name;
    }
    tracks[new_name] = new_track;
    std::cout << "[TrackLibrary] Added track: " << new_name << " with source_type: " << ((source_type==SourceType::Youtube)?"Youtube":"Local") << std::endl;
    return new_name;
}*/

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

