// src/Database.cpp
#include "utils/database.h"
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
            FOREIGN KEY (achievement_id) REFERENCES achievements(id)
        );
    )";

    const char* op_users_table = R"(
        CREATE TABLE IF NOT EXISTS op_users(
            user_id INTEGER PRIMARY KEY,
            permission_level INTEGER NOT NULL,
        );
    )";

    const char* tracks_table = R"(
        CREATE TABLE IF NOT EXISTS tracks(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            sourceType INTEGER NOT NULL,
            source TEXT NOT NULL,
            duration INTEGER NOT NULL,
        );
    )";

    execute_query(achievements_table);
    execute_query(user_achievements_table);
    execute_query(op_users_table);
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

