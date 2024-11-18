//
// Created by Microsoft Köszi on 13/11/2024.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <dpp/snowflake.h>
#include <mutex>
#include <unordered_set>
#include <dpp/dpp.h>

#include <string>
const std::string cookies_path = "./cookies.txt";


struct LocalUser {
    dpp::snowflake usr;
    dpp::snowflake gld;
};



class Globals{
public:
    Globals(const Globals&) = delete;
    Globals& operator=(const Globals&) = delete;

    static Globals& get_instance() {
        static Globals instance;
        return instance;
    }

    void log_user(dpp::snowflake guild_id, dpp::snowflake user_id) {
        std::lock_guard<std::mutex> lock(globals_mutex);

        auto it = logged_users.find(guild_id);
        if (it != logged_users.end()) {
            if (it->second.find(user_id) == it->second.end()) {
                it->second.insert(user_id);
            } else {
                return;
            }
        } else {
            logged_users[guild_id] = {user_id};
        }
    }

    void reg_admin(dpp::snowflake user_id) {
        std::lock_guard<std::mutex> lock(globals_mutex);
        if (admins.find(user_id) != admins.end()) {
            return;
        }
        admins.insert(user_id);
    }

    bool is_log_on(dpp::snowflake user_id, dpp::snowflake guild_id) const {
        std::lock_guard<std::mutex> lock(globals_mutex);

        auto it = logged_users.find(guild_id);
        if (it != logged_users.end()) {
            if (it->second.find(user_id) != it->second.end()) {
                return true;
            }
        }
        return false;
 
    }

    bool is_admin(dpp::snowflake user_id) const {
        std::lock_guard<std::mutex> lock(globals_mutex);
        if (admins.find(user_id) != admins.end()) {
            return true;
        }
        return false;
    }

private:
    Globals() {}
    ~Globals() {}

    mutable std::mutex globals_mutex;

    std::unordered_set<dpp::snowflake> admins;
    std::unordered_map<dpp::snowflake, std::unordered_set<dpp::snowflake>> logged_users;
};


#endif //GLOBALS_H
