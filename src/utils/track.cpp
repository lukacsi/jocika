#include "utils/track.h"
#include <mpg123.h>
#include <iostream>
#include <vector>

Track::Track(const std::string& _name, const std::string& _source, SourceType _source_type)
    : name(_name), source(_source), length_ms(0), rate(0), channels(0), encoding(0) {
    source_type = _source_type;
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

        size_t total_samples = total_bytes / (channels * bytes_per_sample);
        double duration_seconds = static_cast<double>(total_samples) / static_cast<double>(rate);
        length_ms = static_cast<size_t>(duration_seconds * 1000.0);

        std::cout << "[Track] Track data loaded: " << name << std::endl;
       return true;
    } else if (source_type == SourceType::Youtube) {
        // For YouTube tracks, we might fetch metadata if needed
        // Optionally, implement length fetching via yt-dlp
        // For simplicity, assume length_ms is unknown or set by streaming
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
