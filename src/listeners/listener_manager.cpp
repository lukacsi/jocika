#include "listeners/listener_manager.h"

void ListenerManager::add_listener(std::unique_ptr<Listener> listener) {
    listeners.emplace_back(std::move(listener));
}

void ListenerManager::register_listeners(dpp::cluster& bot) {
    for (auto& listener : listeners) {
        listener->register_listener(bot);
    }
}
