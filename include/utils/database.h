#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
public:
    Database(const std::string& db_name);

    ~Database();

    bool execute_query(const std::string& query);
    sqlite3_stmt* prepare_statement(const std::string& query);

    sqlite3* db;
private:
    void initialize_tables();
};

#endif
