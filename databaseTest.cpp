#include <gtest/gtest.h>
#include "database.h"
#include <fstream>
#include <cstdio>

class DatabaseTest : public ::testing::Test {
protected:
    std::string test_db_file = "test.db";
    std::string backup_file = "backup_test.db";

    void SetUp() override {
        std::remove(test_db_file.c_str());
        std::remove(backup_file.c_str());
    }

    void TearDown() override {
        std::remove(test_db_file.c_str());
        std::remove(backup_file.c_str());
    }
};

TEST_F(DatabaseTest, CreateTableAndInsert) {
    Database db(test_db_file);

    std::vector<std::string> columns = {
        "id INTEGER PRIMARY KEY AUTOINCREMENT",
        "name TEXT NOT NULL",
        "age INTEGER NOT NULL"
    };
    db.createTable("users", columns);

    // Try inserting a row
    db.insert("users", { "NULL", "Alice", "30" });

    // Verify the data was inserted (basic verification)
    sqlite3* db_conn;
    sqlite3_open(test_db_file.c_str(), &db_conn);
    sqlite3_stmt* stmt;
    const char* query = "SELECT name, age FROM users WHERE name='Alice';";

    ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(db_conn, query, -1, &stmt, nullptr));
    ASSERT_EQ(SQLITE_ROW, sqlite3_step(stmt));
    EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), "Alice");
    EXPECT_EQ(sqlite3_column_int(stmt, 1), 30);

    sqlite3_finalize(stmt);
    sqlite3_close(db_conn);
}

TEST_F(DatabaseTest, DumpToFileSuccess) {
    Database db(test_db_file);

    std::vector<std::string> columns = {
        "id INTEGER PRIMARY KEY AUTOINCREMENT",
        "name TEXT NOT NULL"
    };
    db.createTable("test_table", columns);
    db.insert("test_table", { "NULL", "testuser" });

    // Dump
    ASSERT_NO_THROW(db.dumpToFile(backup_file));

    // Check if the file was created and is non-empty
    std::ifstream file(backup_file, std::ios::binary);
    ASSERT_TRUE(file.good());
    file.seekg(0, std::ios::end);
    ASSERT_GT(file.tellg(), 0);
}

TEST_F(DatabaseTest, DumpToFileThrowsWhenSourceMissing) {
    Database db("nonexistent.db");
    EXPECT_THROW(db.dumpToFile(backup_file), std::runtime_error);
}

TEST_F(DatabaseTest, DumpToFileThrowsWhenDestInvalid) {
    Database db(test_db_file);
    db.createTable("a", { "id INTEGER" });
    EXPECT_THROW(db.dumpToFile("/invalid/backup.db"), std::runtime_error);
}