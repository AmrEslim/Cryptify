#pragma once

#include <sqlite3.h>
#include <string>

class dataBase
{
public:
    dataBase(const std::string &path);
    ~dataBase();
    dataBase(const dataBase &) = delete;
    dataBase &operator=(const dataBase &) = delete;

private:
    sqlite3 *m_db;
};