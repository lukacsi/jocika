#ifndef PERMISSION_MANAGER_H
#define PERMISSION_MANAGER_H

#include <cstdint>
#include <memory>
#include "database.h"

#define ADMIN_PERMISSION 6004
#define VIP_PERMISSION 6003
#define REGULAR_PERMISSION 6002
#define BASIC_PERMISSION 6001

class PermissionManager {
public:
    PermissionManager(std::shared_ptr<Database> _db) : db(_db) {}
    bool check_permission(uint64_t user_id, int permission_level);
    bool set_permission(uint64_t user_id, int permission_level);

private:
    std::shared_ptr<Database> db;
};

#endif
