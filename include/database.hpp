// ============================================================================
// CRYPTIFY - Database Layer Header
// ============================================================================
// This file provides SQLite database operations for storing users and
// encrypted passwords.
//
// Key concepts:
// - RAII for database connection management
// - Prepared statements to prevent SQL injection
// - BLOB storage for binary data (encrypted passwords, nonces, etc.)
// - Proper error handling
// ============================================================================

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <memory>
#include <sqlite3.h>

// ============================================================================
// STEP 1: DEFINE DATA STRUCTURES
// ============================================================================

// TODO: Define User struct
// Represents a user record from the database
//
// Fields:
// - int id: Unique identifier (primary key)
// - std::string username: Login username
// - std::vector<uint8_t> master_hash: Hashed derived key (for verification)
// - std::vector<uint8_t> salt: Random salt for key derivation
//
// This matches the users table in the database
struct User {
    // TODO: Add fields
    int id;
    std::string username;
    std::vector<uint8_t> master_hash;
    std::vector<uint8_t> salt;
};

// TODO: Define PasswordEntry struct
// Represents a password record from the database
//
// Fields:
// - int id: Unique identifier (primary key)
// - int user_id: Foreign key to users table
// - std::string service: Service name (e.g., "gmail", "github")
// - std::string username: Username for that service
// - std::vector<uint8_t> encrypted_password: The encrypted password bytes
// - std::vector<uint8_t> nonce: Nonce used for encryption (needed for decryption)
// - std::string url: Optional website URL
// - std::string notes: Optional notes
//
// This matches the passwords table in the database
struct PasswordEntry {
    // TODO: Add fields
    int id;
    int user_id;
    std::string service;
    std::string username;
    std::vector<uint8_t> encrypted_password;
    std::vector<uint8_t> nonce;
    std::string url;
    std::string notes;
};

// ============================================================================
// STEP 2: DEFINE DATABASE EXCEPTION
// ============================================================================

// TODO: Define DatabaseException
// Used for all database-related errors
//
// Should inherit from std::runtime_error
class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message)
        : std::runtime_error(message) {}
};

// ============================================================================
// STEP 3: DEFINE DATABASE CLASS
// ============================================================================

class Database {
public:
    // ========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    // ========================================================================
    
    // TODO: Implement constructor
    // Parameters: db_path - Path to SQLite database file
    //
    // What it does:
    // 1. Opens database connection with sqlite3_open()
    // 2. Stores the sqlite3* pointer
    // 3. Throws DatabaseException if open fails
    //
    // Example:
    // Database db("passwords.db");
    explicit Database(const std::string& db_path);
    
    // TODO: Implement destructor
    // What it does:
    // 1. Closes database connection with sqlite3_close()
    // 2. Sets db_ to nullptr
    // 3. Does NOT throw exceptions (destructors shouldn't)
    ~Database();
    
    // ========================================================================
    // DELETE COPY OPERATIONS
    // ========================================================================
    
    // TODO: Delete copy constructor and copy assignment
    // Database connections shouldn't be copied
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    // TODO: You can allow move operations if needed (optional)
    // Database(Database&&) noexcept;
    // Database& operator=(Database&&) noexcept;
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    // TODO: Implement initialize()
    // What it does: Creates the database schema (tables) if they don't exist
    //
    // Steps:
    // 1. Create users table with SQL:
    //    CREATE TABLE IF NOT EXISTS users (
    //        id INTEGER PRIMARY KEY AUTOINCREMENT,
    //        username TEXT UNIQUE NOT NULL,
    //        master_hash BLOB NOT NULL,
    //        salt BLOB NOT NULL,
    //        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    //    );
    //
    // 2. Create passwords table with SQL:
    //    CREATE TABLE IF NOT EXISTS passwords (
    //        id INTEGER PRIMARY KEY AUTOINCREMENT,
    //        user_id INTEGER NOT NULL,
    //        service TEXT NOT NULL,
    //        username TEXT NOT NULL,
    //        encrypted_password BLOB NOT NULL,
    //        nonce BLOB NOT NULL,
    //        url TEXT,
    //        notes TEXT,
    //        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    //        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    //        FOREIGN KEY(user_id) REFERENCES users(id),
    //        UNIQUE(user_id, service)
    //    );
    //
    // 3. Execute both SQL statements
    // 4. Throw DatabaseException on error
    void initialize();
    
    // ========================================================================
    // USER OPERATIONS
    // ========================================================================
    
    // TODO: Implement createUser()
    // What it does: Insert a new user into the database
    //
    // Parameters:
    // - username: Login username (must be unique)
    // - master_hash: Hashed derived key (for verification)
    // - salt: Random salt used for key derivation
    //
    // Steps:
    // 1. Prepare SQL statement:
    //    INSERT INTO users (username, master_hash, salt) VALUES (?, ?, ?)
    //
    // 2. Bind parameters:
    //    - Bind username as TEXT
    //    - Bind master_hash as BLOB
    //    - Bind salt as BLOB
    //
    // 3. Execute with sqlite3_step()
    // 4. Finalize statement
    // 5. Throw DatabaseException on error (e.g., duplicate username)
    //
    // Throws: DatabaseException on failure
    void createUser(
        const std::string& username,
        const std::vector<uint8_t>& master_hash,
        const std::vector<uint8_t>& salt
    );
    
    // TODO: Implement getUser()
    // What it does: Retrieve a user by username
    //
    // Parameters:
    // - username: The username to search for
    //
    // Steps:
    // 1. Prepare SQL statement:
    //    SELECT id, username, master_hash, salt FROM users WHERE username = ?
    //
    // 2. Bind username parameter
    // 3. Execute with sqlite3_step()
    // 4. If SQLITE_ROW returned:
    //    - Read all columns (id, username, master_hash, salt)
    //    - Create User struct
    //    - Return std::optional<User> with the user
    // 5. If SQLITE_DONE returned:
    //    - Return std::nullopt (user not found)
    // 6. Finalize statement
    //
    // Returns: std::optional<User> - contains User if found, empty if not
    // Throws: DatabaseException on database error
    std::optional<User> getUser(const std::string& username);
    
    // TODO: Implement userExists()
    // What it does: Check if a user exists without retrieving full data
    //
    // Steps:
    // 1. Prepare SQL: SELECT COUNT(*) FROM users WHERE username = ?
    // 2. Bind username
    // 3. Execute and read count
    // 4. Return true if count > 0, false otherwise
    //
    // Returns: bool - true if user exists, false otherwise
    bool userExists(const std::string& username);
    
    // ========================================================================
    // PASSWORD OPERATIONS
    // ========================================================================
    
    // TODO: Implement addPassword()
    // What it does: Insert a new password entry into the database
    //
    // Parameters:
    // - entry: PasswordEntry struct with all the data
    //
    // Steps:
    // 1. Prepare SQL statement:
    //    INSERT INTO passwords (user_id, service, username, encrypted_password, 
    //                          nonce, url, notes)
    //    VALUES (?, ?, ?, ?, ?, ?, ?)
    //
    // 2. Bind all parameters:
    //    - user_id as INTEGER
    //    - service as TEXT
    //    - username as TEXT
    //    - encrypted_password as BLOB
    //    - nonce as BLOB
    //    - url as TEXT (can be empty)
    //    - notes as TEXT (can be empty)
    //
    // 3. Execute with sqlite3_step()
    // 4. Finalize statement
    // 5. Throw on error (e.g., duplicate service for this user)
    //
    // Throws: DatabaseException on failure
    void addPassword(const PasswordEntry& entry);
    
    // TODO: Implement getPassword()
    // What it does: Retrieve a password entry by user_id and service name
    //
    // Parameters:
    // - user_id: The user's ID
    // - service: The service name to search for
    //
    // Steps:
    // 1. Prepare SQL:
    //    SELECT id, user_id, service, username, encrypted_password, nonce, 
    //           url, notes
    //    FROM passwords 
    //    WHERE user_id = ? AND service = ?
    //
    // 2. Bind user_id and service
    // 3. Execute with sqlite3_step()
    // 4. If SQLITE_ROW:
    //    - Read all columns
    //    - Create PasswordEntry struct
    //    - Return std::optional<PasswordEntry> with the entry
    // 5. If SQLITE_DONE:
    //    - Return std::nullopt (not found)
    // 6. Finalize statement
    //
    // Returns: std::optional<PasswordEntry> - contains entry if found, empty if not
    // Throws: DatabaseException on database error
    std::optional<PasswordEntry> getPassword(int user_id, const std::string& service);
    
    // TODO: Implement listPasswords()
    // What it does: Get all password entries for a user
    //
    // Parameters:
    // - user_id: The user's ID
    //
    // Steps:
    // 1. Prepare SQL:
    //    SELECT id, user_id, service, username, encrypted_password, nonce,
    //           url, notes
    //    FROM passwords
    //    WHERE user_id = ?
    //    ORDER BY service ASC
    //
    // 2. Bind user_id
    // 3. Create empty vector<PasswordEntry>
    // 4. Loop with sqlite3_step():
    //    - While SQLITE_ROW is returned:
    //      - Read all columns
    //      - Create PasswordEntry
    //      - Add to vector
    // 5. Finalize statement
    // 6. Return the vector
    //
    // Returns: std::vector<PasswordEntry> - all passwords for the user
    // Throws: DatabaseException on database error
    std::vector<PasswordEntry> listPasswords(int user_id);
    
    // TODO: Implement updatePassword()
    // What it does: Update an existing password entry with new encrypted data
    //
    // Parameters:
    // - entry_id: The ID of the password entry to update
    // - encrypted_password: New encrypted password
    // - nonce: New nonce (encryption generates a new nonce each time)
    //
    // Steps:
    // 1. Prepare SQL:
    //    UPDATE passwords 
    //    SET encrypted_password = ?, nonce = ?, updated_at = CURRENT_TIMESTAMP
    //    WHERE id = ?
    //
    // 2. Bind encrypted_password, nonce, and entry_id
    // 3. Execute with sqlite3_step()
    // 4. Check rows affected with sqlite3_changes() (should be 1)
    // 5. Throw if entry_id doesn't exist (0 rows affected)
    // 6. Finalize statement
    //
    // Throws: DatabaseException on failure or if entry doesn't exist
    void updatePassword(
        int entry_id,
        const std::vector<uint8_t>& encrypted_password,
        const std::vector<uint8_t>& nonce
    );
    
    // TODO: Implement deletePassword()
    // What it does: Delete a password entry
    //
    // Parameters:
    // - entry_id: The ID of the password entry to delete
    //
    // Steps:
    // 1. Prepare SQL: DELETE FROM passwords WHERE id = ?
    // 2. Bind entry_id
    // 3. Execute with sqlite3_step()
    // 4. Check rows affected (should be 1)
    // 5. Throw if entry_id doesn't exist
    // 6. Finalize statement
    //
    // Throws: DatabaseException on failure or if entry doesn't exist
    void deletePassword(int entry_id);
    
    // TODO: Implement passwordExists()
    // What it does: Check if a password exists for a service
    //
    // Parameters:
    // - user_id: The user's ID
    // - service: The service name
    //
    // Steps:
    // 1. Prepare SQL: 
    //    SELECT COUNT(*) FROM passwords WHERE user_id = ? AND service = ?
    // 2. Bind user_id and service
    // 3. Execute and read count
    // 4. Return true if count > 0
    //
    // Returns: bool - true if password exists, false otherwise
    bool passwordExists(int user_id, const std::string& service);
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    // TODO: Add private member variable for database handle
    sqlite3* db_;
    
    // ========================================================================
    // PRIVATE HELPER METHODS
    // ========================================================================
    
    // TODO: Implement executeSQL()
    // What it does: Execute simple SQL without parameters
    //
    // Parameters:
    // - sql: The SQL statement to execute
    //
    // Steps:
    // 1. Call sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error_msg)
    // 2. Check return code
    // 3. If error, throw DatabaseException with error message
    // 4. Free error message with sqlite3_free()
    //
    // Used for: CREATE TABLE, simple queries without parameters
    void executeSQL(const std::string& sql);
    
    // TODO: Implement bindBlob()
    // What it does: Bind a vector<uint8_t> as a BLOB parameter
    //
    // Parameters:
    // - stmt: Prepared statement
    // - index: Parameter index (1-based)
    // - data: Vector of bytes to bind
    //
    // Steps:
    // 1. Call sqlite3_bind_blob(stmt, index, data.data(), data.size(), SQLITE_TRANSIENT)
    // 2. SQLITE_TRANSIENT tells SQLite to make a copy of the data
    // 3. Check return code and throw on error
    //
    // Used for: Binding BLOBs (encrypted passwords, nonces, salts, hashes)
    void bindBlob(sqlite3_stmt* stmt, int index, const std::vector<uint8_t>& data);
    
    // TODO: Implement getColumnBlob()
    // What it does: Read a BLOB column from a result row
    //
    // Parameters:
    // - stmt: Statement with current row
    // - column: Column index (0-based)
    //
    // Steps:
    // 1. Get pointer to BLOB data: sqlite3_column_blob(stmt, column)
    // 2. Get BLOB size: sqlite3_column_bytes(stmt, column)
    // 3. Cast pointer to const uint8_t*
    // 4. Create vector<uint8_t> from the data
    // 5. Return the vector
    //
    // Returns: std::vector<uint8_t> containing the BLOB data
    //
    // Used for: Reading encrypted passwords, nonces, salts, hashes
    std::vector<uint8_t> getColumnBlob(sqlite3_stmt* stmt, int column);
};

#endif // DATABASE_HPP

// ============================================================================
// IMPLEMENTATION NOTES
// ============================================================================
//
// After defining this header, implement all methods in database.cpp
//
// Key SQLite concepts:
//
// 1. Prepared Statements:
//    - sqlite3_prepare_v2() - Compile SQL
//    - sqlite3_bind_*() - Bind parameters (prevents SQL injection!)
//    - sqlite3_step() - Execute (returns SQLITE_ROW, SQLITE_DONE, or error)
//    - sqlite3_column_*() - Read result columns
//    - sqlite3_finalize() - Clean up statement
//
// 2. BLOB Handling:
//    - Store binary data (encrypted passwords, nonces, etc.)
//    - Use sqlite3_bind_blob() to insert
//    - Use sqlite3_column_blob() to read
//    - Always check size with sqlite3_column_bytes()
//
// 3. Error Handling:
//    - Always check return codes
//    - Use sqlite3_errmsg(db_) to get error message
//    - Throw DatabaseException with descriptive message
//
// 4. RAII:
//    - Constructor opens database
//    - Destructor closes database
//    - No need to manually close if exception occurs
//
// Testing strategy:
// 1. Create database and initialize schema
// 2. Test user CRUD operations
// 3. Test password CRUD operations
// 4. Test constraints (unique username, duplicate service)
// 5. Test foreign key (delete user - what happens to passwords?)
// 6. Test with invalid inputs
// 7. Use SQLite browser to inspect database file
//
// Security considerations:
// - Always use prepared statements (prevent SQL injection)
// - Validate input before storing
// - Set proper file permissions on database file
// - Consider encrypting the entire database (advanced)
//
// ============================================================================
