#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <dpp/dpp.h>


class AudioUtils {
public:
    static void join_voice_channel(const dpp::slashcommand_t& event);
    static void play_audio();
    static void pause_audio();
    static void stop_audio();
private:
    Audio audio;
};

#endif
