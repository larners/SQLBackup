#include "database.h"
#include <iostream>
#include <fstream>
#include <sstream>

Database::Database(const std::string& db_file): dbFile(db_file)
{
    if (sqlite3_open(dbFile.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Failed to open DB: " << sqlite3_errmsg(db) << "\n";
        db = nullptr;
    }
}

Database::~Database() {
    if (db)
        sqlite3_close(db);
}

void Database::execute_sql(const std::string& sql)
{
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "SQL Error: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
}

void Database::createTable(const std::string& table_name, const std::vector<std::string>& columns)
{
    std::ostringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << table_name << " (";
    for (size_t i = 0; i < columns.size(); ++i)
    {
        ss << columns[i];
        if (i < columns.size() - 1)
            ss << ", ";
    }
    ss << ");";
    execute_sql(ss.str());
}

void Database::insert(const std::string& table_name, const std::vector<std::string>& values) {
    std::ostringstream ss;
    ss << "INSERT INTO " << table_name << " VALUES (";
    for (size_t i = 0; i < values.size(); ++i) {
        ss << "?";
        if (i < values.size() - 1) ss << ", ";
    }
    ss << ");";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return;
    }

    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == "NULL") {
            sqlite3_bind_null(stmt, static_cast<int>(i + 1));
        }
        else {
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), values[i].c_str(), -1, SQLITE_TRANSIENT);
        }
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
}

bool Database::dumpToFile(const std::string& filename)
{
    std::ifstream src_file(dbFile, std::ios::binary);
    std::ofstream dst_file(filename, std::ios::binary);
    if (!src_file || !dst_file)
    {
        std::cerr << "File copy failed.\n";
        return false;
    }
    dst_file << src_file.rdbuf();
    if (!dst_file)
    {
        std::cerr << "Failed to write to file: " << filename << "\n";
        return false;
    }
    return true;
}