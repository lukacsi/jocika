#include "utils/achievement_manager.h"
#include "utils/database.h"
#include <iostream>

AchievementManager::AchievementManager(std::shared_ptr<Database> database) : db(database) {
    load_achievements();
}

// Loads all achievements from the database into the in-memory map
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

// Adds a new achievement to the database and in-memory map
bool AchievementManager::add_achievement(std::shared_ptr<Achievement> achievement) {
    const char* sql = "INSERT INTO achievements (name, description, image_url) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare add achievement statement." << std::endl;
        return false;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, achievement->name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, achievement->description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, achievement->image_url.c_str(), -1, SQLITE_STATIC);

    // Execute
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert achievement: " << sqlite3_errmsg(db->db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Get the last inserted ID
    achievement->id = static_cast<uint64_t>(sqlite3_last_insert_rowid(db->db));

    // Add to in-memory map
    achievements.emplace(achievement->id, achievement);

    sqlite3_finalize(stmt);
    return true;
}

// Assigns an achievement to a user
bool AchievementManager::assign_achievement(uint64_t user_id, uint64_t achievement_id) {
    // Check if achievement exists
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

    // Bind parameters
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int64(stmt, 2, achievement_id);

    // Execute
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to assign achievement: " << sqlite3_errmsg(db->db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

// Retrieves all achievements assigned to a user
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

    // Bind user_id parameter
    sqlite3_bind_int64(stmt, 1, user_id);

    // Execute and fetch results
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

