#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <cstdint>
#include <string>

class Achievement {
public:
    uint64_t id;
    std::string name;
    std::string description;
    std::string image_url;

    Achievement(uint64_t id_ = 0, const std::string& name_ = "", const std::string& description_ = "", const std::string& image_url_ = "");
};

#endif

