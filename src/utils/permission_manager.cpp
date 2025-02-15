#include "utils/permission_manager.h"
#include <iostream>
#include <sqlite3.h>

bool PermissionManager::check_permission(uint64_t user_id, int permission_level) {
    const char* sql = R"(
        SELECT permission_level
        FROM user_permissions
        WHERE user_id = ?;
    )";

    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare check user permission statement!" << std::endl;
        return false;
    }

    sqlite3_bind_int64(stmt, 1, user_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int user_permission = reinterpret_cast<int>(sqlite3_column_int(stmt, 1));
        if (user_permission >= permission_level) {
            return true;
        }
    }
    return false;
}

bool PermissionManager::set_permission(uint64_t user_id, int permission_level) {
    const char* sql = "INSERT OR UPDATE INTO user_permissions (user_id, permission_level) VALUES (?, ?);";
    sqlite3_stmt* stmt = db->prepare_statement(sql);
    if (!stmt) {
        std::cerr << "Failed to prepare user permission set statement!" << std::endl;
        return false;
    }

    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, permission_level);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to assign permission:" << sqlite3_errmsg(db->db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}
