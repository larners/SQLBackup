#pragma once
#include <string>
#include <vector>
#include "sqlite3.h"

class Database {
public:
    explicit Database(const std::string& db_file);
    ~Database();

    void createTable(const std::string& table_name, const std::vector<std::string>& columns);
    void insert(const std::string& table_name, const std::vector<std::string>& values);
    void dumpToFile(const std::string& filename);

private:
    sqlite3* db{ nullptr };
    std::string dbFile;

    void execute_sql(const std::string& sql);
};