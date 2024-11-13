#ifndef TRACK_H
#define TRACK_H

#include <cstdint>
#include <string>
#include <vector>

class Track {
public:
    Track(const std::string& _name, const std::string& _file_path);

    bool load();

    const std::string& get_name() { return name ;}
    const std::vector<uint8_t> get_pcm_data() { return pcm_data; }
    const size_t get_lenght() { return length_ms; }

private:
    std::string name;
    std::string file_path;
    std::vector<uint8_t> pcm_data;
    size_t length_ms;
    long rate;
    int channels;
    int encoding;
};

#endif
