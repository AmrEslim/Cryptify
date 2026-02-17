#include "dBase.hpp"
#include <iostream>
#include <stdexcept>


dataBase::dataBase(const std::string& path) : m_db{nullptr}{
    std::cout << "Initializing database... \n";
    auto exit = sqlite3_open(path.c_str(),&m_db);
    if (exit != SQLITE_OK){
        std::string err = sqlite3_errmsg(m_db);
        sqlite3_close(m_db);
        throw std::runtime_error("failed to open DB");
    }
    const std::string sql = 
        "PRAGMA foreign_keys = ON;" 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash BLOB NOT NULL, "
        "salt BLOB NOT NULL);"
        
        "CREATE TABLE IF NOT EXISTS secrets ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "title TEXT NOT NULL, "
        "encrypted_data BLOB NOT NULL, "
        "iv BLOB NOT NULL, "
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);";
    char* error_msg = nullptr;
    int exec_result = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &error_msg);
    if (exec_result != SQLITE_OK) {
        std::string err(error_msg);
        sqlite3_free(error_msg); 
        sqlite3_close(m_db);     
        throw std::runtime_error("Failed to create tables: " + err);
    }
    
    std::cout << "Database initialized successfully.\n";

};


dataBase::~dataBase(){
    std::cout << "Closing database connection. \n";
    if (m_db){
        sqlite3_close(m_db);
    }
};

bool dataBase::addUser(const std::string &username, const std::vector<uint8_t> &hash, const std::vector<uint8_t> &salt){
    std::cout << "adding a new user to the database. \n";
    const char* sql = "INSERT INTO users (username, password_hash, salt) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt; ////what is this ??
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; 
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 2, hash.data(), hash.size(), SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 3, salt.data(), salt.size(), SQLITE_TRANSIENT);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    
    return success;    
}
bool dataBase::PrintUser(const std::string &username){
    std::cout << "searching for user in the database. \n";
    sqlite3_stmt* stmt;
    const char* sql = "SELECT username, password_hash, salt FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; 
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    if(sqlite3_step(stmt) == SQLITE_ROW){
        std::cout << "username : " << sqlite3_column_text(stmt, 0 ) << "\n";
        std::string outHash ="";
        const void* hashBlob = sqlite3_column_blob(stmt, 1);
        int hashSize = sqlite3_column_bytes(stmt, 1);
        outHash.assign(reinterpret_cast<const char*>(hashBlob), hashSize);
        std::cout << "hash : " << outHash<< "\n";
        std::cout << "salt : " << sqlite3_column_blob(stmt, 2 )<< "\n";
        sqlite3_finalize(stmt);
        return true;
    }
    std::cout << "User not found.\n";
    sqlite3_finalize(stmt);
    return false;
}


bool dataBase::getUser(const std::string &username, UserQuerey &outData){
    sqlite3_stmt *stmt;
    const char* sql= "SELECT id, password_hash, salt FROM users WHERE username = ?;";
    if(sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    if(sqlite3_step(stmt) == SQLITE_ROW){  
        std::cout << "found user...\n";  
        outData.id = sqlite3_column_int(stmt, 0 );
        std::string outHash ="";
        const void* hashBlob = sqlite3_column_blob(stmt, 1);
        int hashSize = sqlite3_column_bytes(stmt, 1);
        if (hashBlob && hashSize > 0) {
            const uint8_t* data = static_cast<const uint8_t*>(hashBlob);
            outData.hash.assign(data, data + hashSize);
        }
        const void* saltBlob = sqlite3_column_blob(stmt, 2);
        int saltSize = sqlite3_column_bytes(stmt, 2);
        if (saltBlob && saltSize > 0) {
            const uint8_t* data = static_cast<const uint8_t*>(saltBlob);
            outData.salt.assign(data, data + saltSize);
        }
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;

};

bool dataBase::addSecret(int userId, const std::string &title, const std::vector<uint8_t> &encryptedData, const std::vector<uint8_t> &iv){
    std::cout << "adding a new user to the database. \n";
    const char* sql = "INSERT INTO secrets (user_id, title, encrypted_data, iv) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt; ////what is this ??
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; 
    }
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 3, encryptedData.data(), encryptedData.size(), SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 4, iv.data(), iv.size(), SQLITE_TRANSIENT);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    
    return success;  
};

std::vector<dataBase::secretRecord> dataBase::getSecrets(int userId){
    sqlite3_stmt *stmt;
    std::vector<secretRecord> results;

    const char* sql= "SELECT title, encrypted_data, iv FROM secrets WHERE user_id = ?;";
    if(sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        return results;
    }
    sqlite3_bind_int(stmt, 1, userId);
    while(sqlite3_step(stmt) == SQLITE_ROW){ 
        secretRecord record; 
        record.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const void* encryptedDataBlob = sqlite3_column_blob(stmt, 1);
        int encryptedDataSize = sqlite3_column_bytes(stmt, 1);
        if (encryptedDataBlob && encryptedDataSize > 0) {
            const uint8_t* data = static_cast<const uint8_t*>(encryptedDataBlob);
            record.encryptedData.assign(data, data + encryptedDataSize);
        }
        const void* ivBlob = sqlite3_column_blob(stmt, 2);
        int ivSize = sqlite3_column_bytes(stmt, 2);
        if (ivBlob && ivSize > 0) {
            const uint8_t* data = static_cast<const uint8_t*>(ivBlob);
            record.iv.assign(data, data + ivSize);
        }
        results.push_back(record);
        std::cout << record.title << "\n";

    }
    sqlite3_finalize(stmt);
    return results;

};