#pragma once
#include "database.h"


class usersTableHelper
{
public:
    usersTableHelper(Database& db);
    void fillTable(int count = 100);
public:
    usersTableHelper(const usersTableHelper&) = delete;
    usersTableHelper& operator=(const usersTableHelper&) = delete;
private:
    Database& db_;
};