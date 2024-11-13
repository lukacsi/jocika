//
// Created by Microsoft Köszi on 13/11/2024.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <dpp/dpp.h>

#include <string>
const std::string cookies_path = "./cookies.txt";


struct LocalUser {
    dpp::snowflake usr;
    dpp::snowflake gld;
};


class Globals{
public:
    static void log_user(dpp::snowflake u, dpp::snowflake g) {
        LocalUser lu;
        lu.gld = g;
        lu.usr = u;
        LUs.push_back(lu);
    }
    static int get_v_size() {
        return LUs.size();
    }
    static dpp::snowflake get_user(int i) {
        return LUs.at(i).usr;
    }
    static dpp::snowflake get_guild(int i) {
        return LUs.at(i).gld;
    }
//private:
    static std::vector<LocalUser> LUs;
};

extern Globals go;


#endif //GLOBALS_H
