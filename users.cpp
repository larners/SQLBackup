#include "users.h"
#include <string>
#include <cstdlib>
#include <ctime>

usersTableHelper::usersTableHelper(Database& db): db_(db)
{
    db_.createTable("users", {
        "id INTEGER PRIMARY KEY AUTOINCREMENT",
        "name TEXT NOT NULL",
        "age INTEGER NOT NULL"
        });
}

void usersTableHelper::fillTable(int count) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < count; ++i)
    {
        std::string name = "User" + std::to_string(rand() % 10000);
        std::string age = std::to_string(18 + rand() % 50);
        db_.insert("users", { "NULL", name, age });
    }
}