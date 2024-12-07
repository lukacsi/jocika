// src/Database.cpp
#include "utils/database.h"
#include "globals.h"
#include <iostream>

Database::Database(const std::string& db_name) : db(nullptr) {
    if (sqlite3_open(db_name.c_str(), &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
    } else {
        initialize_tables();
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

void Database::initialize_tables() {
    const char* achievements_table = R"(
        CREATE TABLE IF NOT EXISTS achievements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            description TEXT NOT NULL,
            image_url TEXT NOT NULL
        );
    )";

    const char* user_achievements_table = R"(
        CREATE TABLE IF NOT EXISTS user_achievements (
            user_id INTEGER NOT NULL,
            achievement_id INTEGER NOT NULL,
            PRIMARY KEY (user_id, achievement_id),
            FOREIGN KEY (user_id) REFERENCES op_users(user_id),
            FOREIGN KEY (achievement_id) REFERENCES achievements(id)
        );
    )";

    const char* op_users_table = R"(
        CREATE TABLE IF NOT EXISTS op_users (
            user_id INTEGER PRIMARY KEY,
            permission_level INTEGER NOT NULL
        );
    )";

    const char* tracks_table = R"(
        CREATE TABLE IF NOT EXISTS tracks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            sourceType INTEGER NOT NULL,
            source TEXT NOT NULL,
            duration INTEGER NOT NULL
        );
    )";

    execute_query(achievements_table);
    execute_query(user_achievements_table);
    execute_query(op_users_table);
    execute_query(tracks_table);
    insert_initial_entries();
}

sqlite3_stmt* Database::prepare_statement(const std::string& query) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return stmt;
}

bool Database::execute_query(const std::string& query) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

void Database::insert_initial_entries() {
    const char* insert_achievements = R"(
        INSERT INTO achievements (name, description, image_url) VALUES
            ('First Login', 'Awarded upon the first login', 'http://example.com/images/first_login.png'),
            ('Top Contributor', 'Awarded to top contributors', 'http://example.com/images/top_contributor.png'),
            ('Marathon Runner', 'Completed 100 tasks', 'http://example.com/images/marathon_runner.png')
        ON CONFLICT(name) DO NOTHING;
    )";

    if (!execute_query(insert_achievements)) {
        std::cerr << "Failed to insert achievements." << std::endl;
    }

    const std::string insert_op_users_query = R"(
        INSERT INTO op_users (user_id, permission_level) VALUES
            (?, 4)
        ON CONFLICT(user_id) DO NOTHING;
    )";

    sqlite3_stmt* stmt = prepare_statement(insert_op_users_query);
    if (!stmt) {
        std::cerr << "Failed to prepare insert_op_users statement." << std::endl;
        return;
    }

    for (const uint64_t& uid : owners) {
        int rc = sqlite3_bind_int64(stmt, 1, uid);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind user_id " << uid << ": " << sqlite3_errmsg(db) << std::endl;
            sqlite3_reset(stmt);
            continue;
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Execution failed for user_id " << uid << ": " << sqlite3_errmsg(db) << std::endl;
        }

        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to reset statement after inserting user_id " << uid << ": " << sqlite3_errmsg(db) << std::endl;
        }
    }

    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        std::cerr << "Failed to finalize insert_op_users statement: " << sqlite3_errmsg(db) << std::endl;
    }
}
