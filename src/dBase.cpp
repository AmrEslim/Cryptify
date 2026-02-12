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