#include "utils/achievement.h"

Achievement::Achievement(uint64_t id_, const std::string& name_, const std::string& description_, const std::string& image_url_)
    : id(id_), name(name_), description(description_), image_url(image_url_) {}

