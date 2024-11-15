#ifndef TRACK_H
#define TRACK_H

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

enum class SourceType {
    Local,
    Youtube
};

class Track {
public:
    Track(const std::string& _name, const std::string& _source, SourceType _source_type);

    bool load();
    void unload();

    const std::string& get_name() { return name ;}
    const std::vector<uint8_t> get_pcm_data() { return pcm_data; }
    const size_t get_length() { return length_ms; }
    SourceType get_source_type() { return source_type; }
    const std::string& get_source() { return source; }
    const std::string toString() { 
            size_t total_seconds = length_ms / 1000;
        size_t hours = total_seconds / 3600;
        size_t minutes = (total_seconds % 3600) / 60;
        size_t seconds = total_seconds % 60;

        std::ostringstream oss;
        oss << name << " (" 
            << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << seconds << ")";
        return oss.str();
    }

private:
    std::string name;
    std::string source;
    SourceType source_type;
    std::vector<uint8_t> pcm_data;
    size_t length_ms;
    long rate;
    int channels;
    int encoding;
};

#endif
