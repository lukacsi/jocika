#ifndef ACHIEVEMENTMANAGER_H
#define ACHIEVEMENTMANAGER_H

#include "achievement.h"
#include "database.h"
#include <memory>
#include <vector>
#include <map>
#include <cstdint>

class AchievementManager {
public:
    AchievementManager(std::shared_ptr<Database> database);

    void load_achievements();

    std::shared_ptr<Achievement> add_achievement(std::shared_ptr<Achievement> achievement);

    bool assign_achievement(uint64_t user_id, uint64_t achievement_id);

    std::vector<std::shared_ptr<Achievement>> get_user_achievements(uint64_t user_id);

    std::vector<std::shared_ptr<Achievement>> get_all_achievements();

    std::shared_ptr<Achievement> get_achievement(uint64_t id);

    float get_completion_stats(uint64_t achievement_id) const;

private:
    std::shared_ptr<Database> db;
    std::map<uint64_t, std::shared_ptr<Achievement>> achievements;
};

#endif

