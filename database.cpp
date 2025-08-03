#include "database.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

Database::Database(const std::string& path) : db(nullptr), initialized(false), dbPath(path) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << "\n";
        db = nullptr;
    }
    else
    {
        initialized = true;
        if (!createTableUsers())
            std::cerr << "Failed to create table.\n";
    }
}

Database::~Database() {
    if (db)
        sqlite3_close(db);
}

Database& Database::getInstance(const std::string& dbPath) {
    static Database instance(dbPath);
    return instance;
}

bool Database::createTableUsers() {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "age INTEGER);";
    return execute(sql);
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::insertRandomRows(int count) {
    if (!initialized) 
        return false;

    const char* sql = "INSERT INTO users (name, age) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)// TODO: have a look
    {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    for (int row = 0; row < count; ++row)
    {
        std::string name = "User" + std::to_string(rand() % 10000);
        int age = 18 + rand() % 50;

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, age);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            std::cerr << "Insert failed at row " << row << ": " << sqlite3_errmsg(db) << "\n";
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::exportToFile(const std::string& exportPath) {
    if (!initialized) return false;

    std::ifstream src(dbPath, std::ios::binary);
    std::ofstream dst(exportPath, std::ios::binary);

    if (!src || !dst) {
        std::cerr << "Failed to open files for copying.\n";
        return false;
    }

    dst << src.rdbuf(); // Copy contents
    std::cout << "Database copied to : " << exportPath << "\n";
    return true;
}

