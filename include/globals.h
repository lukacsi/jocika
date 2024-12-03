//
// Created by Microsoft Köszi on 13/11/2024.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>
#include <dpp/snowflake.h>
#include <mutex>
#include <unordered_set>
#include <dpp/dpp.h>
#include <string>

const std::string cookies_path = "./cookies.txt";
const int max_tracks_reply = 30;
const std::vector<uint64_t> owners = {274276440642551818, 385139194391166989};

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

    void my_logger(dpp::snowflake user_id) {
        std::lock_guard<std::mutex> lock(globals_mutex);
        targets.insert(user_id);
    }

    bool my_log_on(dpp::snowflake user_id) {
        std::lock_guard<std::mutex> lock(globals_mutex);
        if(targets.find(user_id) != targets.end()) {
            return true;
        }
            return false;

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

    FILE* vc_file() {
        if(vr_on){
            return fd;
        }
        fd = fopen("./me.pcm", "wb");
        vr_on = true;
        return fd;
    }
    void vc_file_close() {
        fclose(fd);
    }
    void vr_enable() {
        vr_on = true;
    }
    void vr_disable() {
        vr_on = false;
    }
    bool is_vr_on() {
        return vr_on;
    }

private:
    Globals() {}
    ~Globals() {}

    mutable std::mutex globals_mutex;

    FILE *fd;
    bool vr_on;

    std::unordered_set<dpp::snowflake> admins;
    std::unordered_set<dpp::snowflake> targets;
    std::unordered_map<dpp::snowflake, std::unordered_set<dpp::snowflake>> logged_users;
};


#endif //GLOBALS_H
