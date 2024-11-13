#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <dpp/dpp.h>


class AudioUtils {
public:
    static bool join_voice_channel(const dpp::slashcommand_t& event);
};

#endif
