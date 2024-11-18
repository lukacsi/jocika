#include "utils/track.h"
#include "globals.h"
#include <memory>
#include <array>
#include <mpg123.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using std::shared_ptr;

Track::Track(const std::string& _name, const std::string& _source, SourceType _source_type)
    : name(_name), source(_source), length(0), rate(0), channels(0),  encoding(0) {
    source_type = _source_type;
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

bool Track::init() {
    if (source_type == SourceType::Local) {
        if (mpg123_init() != MPG123_OK) {
            std::cerr << "[TrackLibrary] Failed to initialize mpg123" << std::endl;
        }

        mpg123_handle* mh = mpg123_new(NULL, NULL);
        if (!mh) {
            std::cerr << "[Track] Failed to create mpg123 handle." << std::endl;
            mpg123_exit();
            return false;
        }

        mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

        if (mpg123_open(mh, source.c_str()) != MPG123_OK) {
            std::cerr << "[Track] Failed to open file: " << source << " " << mpg123_strerror(mh) << std::endl;
            mpg123_delete(mh);
            mpg123_exit();
            return false;
        }

        if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
            std::cerr << "[Track] Bad format: " << mpg123_strerror(mh) << std::endl;
            mpg123_close(mh);
            mpg123_delete(mh);
            mpg123_exit();
            return false;
        }

        size_t buffer_size = mpg123_outblock(mh);
        unsigned char* buffer = new unsigned char[buffer_size];
        size_t done = 0;

        size_t total_bytes = 0;
        while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
            pcm_data.insert(pcm_data.end(), buffer, buffer + done);
            total_bytes += done;
        }

        if (mpg123_close(mh) != MPG123_OK) {
            std::cerr << "[Track] Failed to close file: " << source << std::endl;
            mpg123_delete(mh);
            delete[] buffer;
            mpg123_exit();
            return false;
        }

        mpg123_delete(mh);
        delete[] buffer;
        mpg123_exit();

        int bytes_per_sample = mpg123_encsize(encoding);
        if (bytes_per_sample <= 0) {
            std::cerr << "[Track] Unsupported encoding." << std::endl;
            return false;
        }

        size_t total_samples = total_bytes / (channels * bytes_per_sample);
        double duration_seconds = static_cast<double>(total_samples) / static_cast<double>(rate);
        length = static_cast<size_t>(duration_seconds);
        length += 1;

        std::cout << "[Track] Track data initialized: " << name << std::endl;
        pcm_data.clear();
        return true;
    } else if (source_type == SourceType::Youtube) {

        std::array<char, 128> buffer;
        std::string data;
        std::string command = "yt-dlp --cookies '" + cookies_path + "' --get-title --get-duration -F '" + source + "'";
        shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) {
            std::cerr << "[Track] popen() failed!" << std::endl;
            return false;
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            data+= buffer.data();
        }

        //std::cout << data << std::endl;

        if (data.length() < 1) {
            return false;
        }

        std::vector<std::string> data_lines;
        std::istringstream stream(data);
        std::string line;
        while (std::getline(stream, line)) {
            data_lines.push_back(line);
        }

        std::string time_data = trim(data_lines[data_lines.size() - 1]);
        data_lines.pop_back();
        std::string name_data = trim(data_lines[data_lines.size() - 1]);
        name = name_data;
        data_lines.pop_back();
        std::regex data_line_regex(R"(^\s*(\d+)\s+)");

        std::vector<std::string> formats;

        for (size_t i = 0; i < data_lines.size(); ++i) {
            const std::string& current_line = data_lines[i];

            if (std::regex_search(current_line, data_line_regex)) {
                std::vector<std::string> tokens = split(current_line);
                if (!tokens.empty()) {
                    formats.push_back(tokens[0]);
                }
            }
        }

        std::vector<size_t> time_parts;
        std::stringstream tsream(time_data);
        std::string part;

        while (std::getline(tsream, part, ':')) {
            time_parts.push_back(std::stoi(part));
        }
        if (time_parts.size() == 1) {
            length = time_parts[0];
        } else if (time_parts.size() == 2) {
            length = time_parts[0] * 60 + time_parts[1];
        } else if (time_parts.size() == 3) {
            length = time_parts[0] * 3600 + time_parts[1] * 60 + time_parts[2];
        } else {
            std::cerr << "Unexpected duration format: " << time_data << std::endl;
            return false;
        }
        length += 1;
        best_format = select_best_audio_format(formats);

        std::cout << "[Track] Initialized YouTube track '" << name << "' duration " << length << " best format " << best_format << std::endl;
        return true;
    }
    return false;

}

bool Track::load() {
    if (source_type == SourceType::Local) {
        if (mpg123_init() != MPG123_OK) {
            std::cerr << "[TrackLibrary] Failed to initialize mpg123" << std::endl;
        }

        mpg123_handle* mh = mpg123_new(NULL, NULL);
        if (!mh) {
            std::cerr << "[Track] Failed to create mpg123 handle." << std::endl;
            mpg123_exit();
            return false;
        }

        mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

        if (mpg123_open(mh, source.c_str()) != MPG123_OK) {
            std::cerr << "[Track] Failed to open file: " << source << " " << mpg123_strerror(mh) << std::endl;
            mpg123_delete(mh);
            mpg123_exit();
            return false;
        }

        if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
            std::cerr << "[Track] Bad format: " << mpg123_strerror(mh) << std::endl;
            mpg123_close(mh);
            mpg123_delete(mh);
            mpg123_exit();
            return false;
        }

        size_t buffer_size = mpg123_outblock(mh);
        unsigned char* buffer = new unsigned char[buffer_size];
        size_t done = 0;

        size_t total_bytes = 0;
        while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
            pcm_data.insert(pcm_data.end(), buffer, buffer + done);
            total_bytes += done;
        }

        if (mpg123_close(mh) != MPG123_OK) {
            std::cerr << "[Track] Failed to close file: " << source << std::endl;
            mpg123_delete(mh);
            delete[] buffer;
            mpg123_exit();
            return false;
        }

        mpg123_delete(mh);
        delete[] buffer;
        mpg123_exit();

        int bytes_per_sample = mpg123_encsize(encoding);
        if (bytes_per_sample <= 0) {
            std::cerr << "[Track] Unsupported encoding." << std::endl;
            return false;
        }

        std::cout << "[Track] Track data loaded: " << name << std::endl;
        return true;
    } else if (source_type == SourceType::Youtube) {
        std::cout << "[Track] Ready to stream YouTube track '" << name << "'." << std::endl;
        return true;
    }
    return false;
}

void Track::unload() {
    if (source_type == SourceType::Local) {
        pcm_data.clear();
        std::cout << "[Track] Track data unloaded: " << name << std::endl;
    }
}
