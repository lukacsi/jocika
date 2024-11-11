#ifndef LISTENER_H
#define LISTENER_H

#include <dpp/dpp.h>

class Listener {
public:
    virtual ~Listener() = default;
    virtual void register_listener(dpp::cluster& bot) = 0;
};

#endif
