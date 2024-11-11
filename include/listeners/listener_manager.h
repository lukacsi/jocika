#ifndef LISTENER_MANAGER_H
#define LISTENER_MANAGER_H

#include "listener.h"
#include <vector>
#include <memory>

class ListenerManager {
public:
    void add_listener(std::unique_ptr<Listener> listener);
    void register_listeners(dpp::cluster& bot);

private:
    std::vector<std::unique_ptr<Listener>> listeners;
};

#endif
