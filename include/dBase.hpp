#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <cstdint>

class dataBase
{
public:
    struct UserQuerey
    {
        int id;
        std::vector<uint8_t> hash;
        std::vector<uint8_t> salt;
    };
    struct secretRecord
    {
        std::string title;
        std::vector<uint8_t> encryptedData;
        std::vector<uint8_t> iv;
    };
    dataBase(const std::string &path);
    ~dataBase();
    dataBase(const dataBase &) = delete;
    dataBase &operator=(const dataBase &) = delete;

    bool addUser(const std::string &username, const std::string &hash, const std::string &salt);
    bool PrintUser(const std::string &username); // just for testing .....
    bool getUser(const std::string &username, UserQuerey &uoutData);
    bool addSecret(int userId, const std::string &title, const std::vector<uint8_t> &encryptedData, const std::vector<uint8_t> &iv);
    std::vector<secretRecord> getSecrets(int userId);

private:
    sqlite3 *m_db;
};