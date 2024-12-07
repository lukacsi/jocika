#include "utils/achievement_manager.h"
#include "utils/database.h"
#include <iostream>

AchievementManager::AchievementManager(std::shared_ptr<Database> database) : db(database) {
    load_achievements();
}

void AchievementManager::load_achievements() {
    const char* sql = "SELECT id, name, description, image_url FROM achievements;";
    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare achievements select statement." << std::endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        uint64_t id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        auto achievement = std::make_shared<Achievement>(id, name, description, image_url);
        achievements.emplace(id, achievement);
    }

    sqlite3_finalize(stmt);
}

std::shared_ptr<Achievement> AchievementManager::add_achievement(std::shared_ptr<Achievement> achievement) {
    const char* sql = "INSERT INTO achievements (name, description, image_url) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare add achievement statement." << std::endl;
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, achievement->name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, achievement->description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, achievement->image_url.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert achievement: " << sqlite3_errmsg(db->db) << std::endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    achievement->id = static_cast<uint64_t>(sqlite3_last_insert_rowid(db->db));

    achievements.emplace(achievement->id, achievement);

    sqlite3_finalize(stmt);
    return achievement;
}

bool AchievementManager::assign_achievement(uint64_t user_id, uint64_t achievement_id) {
    if (achievements.find(achievement_id) == achievements.end()) {
        std::cerr << "Achievement ID " << achievement_id << " does not exist." << std::endl;
        return false;
    }

    const char* sql = "INSERT OR IGNORE INTO user_achievements (user_id, achievement_id) VALUES (?, ?);";
    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare assign achievement statement." << std::endl;
        return false;
    }

    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int64(stmt, 2, achievement_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to assign achievement: " << sqlite3_errmsg(db->db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<std::shared_ptr<Achievement>> AchievementManager::get_user_achievements(uint64_t user_id) {
    std::vector<std::shared_ptr<Achievement>> user_achs;
    const char* sql = R"(
        SELECT a.id, a.name, a.description, a.image_url
        FROM achievements a
        JOIN user_achievements ua ON a.id = ua.achievement_id
        WHERE ua.user_id = ?;
    )";

    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare get user achievements statement." << std::endl;
        return user_achs;
    }

    sqlite3_bind_int64(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        uint64_t id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        auto achievement = std::make_shared<Achievement>(id, name, description, image_url);
        user_achs.push_back(achievement);
    }

    sqlite3_finalize(stmt);
    return user_achs;
}

std::vector<std::shared_ptr<Achievement>> AchievementManager::get_all_achievements() {
    std::vector<std::shared_ptr<Achievement>> ach;
    const char* sql = R"(
        SELECT id, name, description, image_url
        FROM achievements;
    )";

    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "failed to prepare get achievements statement." << std::endl;
        return ach;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        uint64_t id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        auto achievement = std::make_shared<Achievement>(id, name, description, image_url);
        ach.push_back(achievement);
    }

    sqlite3_finalize(stmt);
    return ach;
}
std::shared_ptr<Achievement> AchievementManager::get_achievement(uint64_t id) {
    std::shared_ptr<Achievement> achievement = nullptr;
    const char* sql = R"(
        SELECT id, name, description, image_url
        FROM achievements
        WHERE id = ?;
    )";

    auto stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "failed to prepare get achievements statement." << std::endl;
        return achievement;
    }

    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        uint64_t id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        achievement = std::make_shared<Achievement>(id, name, description, image_url);
    }
    else {
        std::cout << "Achievement not fount\n";
    }

    sqlite3_finalize(stmt);
    return achievement;
}

float AchievementManager::get_completion_stats(uint64_t achievement_id) const  {
    const char* total_count = R"(
        SELECT COUNT(DISTINCT user_id)
        FROM user_achievements;
    )";
    const char* specific_count = R"(
        SELECT COUNT(*)
        FROM user_achievements
        WHERE achievement_id = ?;
    )";

    auto total_stmt = db->prepare_statement(total_count);
    if (!total_stmt) {
        std::cerr << "Failed to prepare get total count statement." << std::endl;
        return -1;
    }

    auto specific_stmt = db->prepare_statement(specific_count);
    if (!specific_stmt) {
        std::cerr << "Failed to prepare get specific count statement." << std::endl;
        return -1;
    }

    int total = 0;
    while (sqlite3_step(total_stmt) == SQLITE_ROW) {
        total = reinterpret_cast<int>(sqlite3_column_int(total_stmt, 0));
    }
    sqlite3_finalize(total_stmt);

    sqlite3_bind_int64(specific_stmt, 1, achievement_id);
    int specific = 0;
    while (sqlite3_step(specific_stmt) == SQLITE_ROW) {
        specific = reinterpret_cast<int>(sqlite3_column_int(specific_stmt, 0));
    }
    sqlite3_finalize(specific_stmt);
    
    if (specific <= 0 || total <= 0) {
        std::cout << "Achievement not completed in db" << std::endl;
        return 0;
    } else if (specific > total) {
        std::cerr << "More specific occurences of achievemnt than ids, cant happen logically" << std::endl;
        return -1;
    }

    return static_cast<float>(specific) / static_cast<float>(total);
}

