#pragma once
#include <string>
#include "sqlite3.h"

class Database {
public:
    static Database& getInstance(const std::string& dbPath = "");

    bool insertRandomRows(int count);
    bool exportToFile(const std::string& exportPath);
private:
    Database(const std::string& path);
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool createTableUsers();
    bool execute(const std::string& sql);

    sqlite3* db;
    bool initialized;
    std::string dbPath;
};
