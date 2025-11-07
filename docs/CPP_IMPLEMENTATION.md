# C++ Implementation Guide - Cryptify

## Overview
This guide will help you implement the C++ backend for Cryptify, which includes:
- **Crypto Wrapper**: Safe C++ interface to Rust cryptographic functions
- **Database Layer**: SQLite integration for data persistence
- **Password Manager**: Business logic orchestrating everything
- **CLI Interface**: Command-line user interface

---

## Table of Contents
1. [Phase Overview](#phase-overview)
2. [Step 1: Crypto Wrapper](#step-1-crypto-wrapper)
3. [Step 2: Database Layer](#step-2-database-layer)
4. [Step 3: Password Manager Logic](#step-3-password-manager-logic)
5. [Step 4: CLI Interface](#step-4-cli-interface)
6. [Testing Strategy](#testing-strategy)
7. [Common Issues](#common-issues)

---

## Phase Overview

### The Architecture
```
┌─────────────────────────────────────┐
│         main.cpp (CLI)              │  ← You interact here
└─────────────┬───────────────────────┘
              │
┌─────────────▼───────────────────────┐
│    PasswordManager Class            │  ← Orchestrates everything
│  - Manages user session             │
│  - Coordinates crypto + database    │
└────┬────────────────────┬───────────┘
     │                    │
┌────▼──────────┐  ┌──────▼──────────┐
│ CryptoEngine  │  │   Database      │
│  (Wrapper)    │  │   (SQLite)      │
│               │  │                 │
│  Wraps Rust ──┼──┤  Stores data    │
│  functions    │  │  encrypted      │
└───────────────┘  └─────────────────┘
```

### Implementation Order
1. **CryptoEngine** (Week 1-2): Wrap Rust functions safely
2. **Database** (Week 2-3): SQLite integration and CRUD operations
3. **PasswordManager** (Week 3-4): Business logic and session management
4. **CLI** (Week 4-5): User interface and command parsing

---

## Step 1: Crypto Wrapper

### Goal
Create a safe, idiomatic C++ interface to call your Rust cryptographic functions.

### Learning Objectives
- Understand FFI (Foreign Function Interface)
- Learn RAII (Resource Acquisition Is Initialization)
- Practice modern C++ error handling
- Convert between C++ and C types safely

### Files to Create
- `cpp-core/include/crypto_wrapper.hpp` - Header with class declaration
- `cpp-core/src/crypto_wrapper.cpp` - Implementation

### Key Concepts

#### 1.1 Why Wrap the Rust Functions?
**Raw Rust FFI functions are:**
- Using raw pointers (unsafe)
- Using C types (not idiomatic C++)
- Returning error codes (not exceptions)
- Hard to use correctly

**Your C++ wrapper should:**
- Use `std::string` and `std::vector<uint8_t>`
- Use RAII for resource management
- Throw exceptions on errors
- Provide type safety

#### 1.2 Type Conversions You'll Need

| Rust (FFI)          | C Type          | C++ Type                |
|---------------------|-----------------|-------------------------|
| `*const c_char`     | `const char*`   | `std::string`           |
| `*const u8`         | `const uint8_t*`| `std::vector<uint8_t>`  |
| `*mut u8`           | `uint8_t*`      | `std::vector<uint8_t>`  |
| `usize`             | `size_t`        | `size_t`                |
| `i32`               | `int`           | `int`                   |
| `bool`              | `bool`          | `bool`                  |

#### 1.3 Class Design

**CryptoEngine class should:**
- Be non-copyable (crypto state shouldn't be copied)
- Use default constructor (no state needed)
- Have methods for each Rust function
- Convert types automatically
- Throw meaningful exceptions on errors

#### 1.4 Error Handling Strategy

**Option 1: Exceptions (Recommended)**
```
Rust returns error code → C++ throws exception
Benefits: Clean code, automatic cleanup, idiomatic
```

**Option 2: std::optional / std::expected**
```
Rust returns error code → C++ returns optional/expected
Benefits: No exceptions, explicit error handling
```

**For this project, use exceptions** - they're more natural for C++ and ensure cleanup.

### Implementation Tasks

#### Task 1.1: Declare Rust Functions (crypto_wrapper.hpp)
- Add `extern "C"` block with all Rust function signatures
- Must match exactly what Rust exports
- Use C types in the extern block

#### Task 1.2: Define Custom Exceptions (crypto_wrapper.hpp)
- Create exception classes:
  - `CryptoException` (base class)
  - `KeyDerivationError`
  - `EncryptionError`
  - `DecryptionError`
  - `RandomGenerationError`

#### Task 1.3: Create CryptoEngine Class (crypto_wrapper.hpp)
- Define the class interface
- Private: constants (KEY_SIZE, SALT_SIZE, etc.)
- Public: methods for each operation
- Delete copy constructor and assignment operator

#### Task 1.4: Implement Each Wrapper Method (crypto_wrapper.cpp)

**For each method:**
1. Convert C++ types to C types (string → char*, vector → pointer)
2. Allocate buffers for output
3. Call the Rust function
4. Check return code
5. If error: throw exception with meaningful message
6. If success: convert C types back to C++ types
7. Return the result

**Methods to implement:**
- `generateSalt()` → `std::vector<uint8_t>`
- `generateNonce()` → `std::vector<uint8_t>`
- `generatePassword(length, flags)` → `std::string`
- `deriveKey(password, salt)` → `std::vector<uint8_t>`
- `hashKeyForStorage(key)` → `std::vector<uint8_t>`
- `encrypt(plaintext, key)` → `std::pair<std::vector<uint8_t>, std::vector<uint8_t>>`
- `decrypt(ciphertext, key, nonce)` → `std::string`

### Security Considerations

#### 1.5 Memory Safety at FFI Boundary
- Always validate buffer sizes before calling Rust
- Never pass a pointer to temporary data
- Ensure buffers live until Rust call completes
- Use `std::vector` for automatic memory management

#### 1.6 Sensitive Data Handling
- Consider using `explicit_bzero()` or similar to clear sensitive buffers
- Be aware: `std::string` and `std::vector` don't automatically zero memory
- For production: implement a secure allocator or use libraries like libsodium's secure memory

### Testing Plan

**Test each wrapper method:**
1. Call it from a test program
2. Verify it returns expected results
3. Test error cases (empty input, null data, etc.)
4. Verify exceptions are thrown on errors
5. Check for memory leaks (use Valgrind or ASAN)

---

## Step 2: Database Layer

### Goal
Implement SQLite database operations to store users and encrypted passwords.

### Learning Objectives
- Learn SQLite C API
- Practice RAII for database connections
- Understand prepared statements (SQL injection prevention)
- Work with binary data (BLOBs)

### Files to Create
- `cpp-core/include/database.hpp` - Header with Database class and structs
- `cpp-core/src/database.cpp` - Implementation

### Key Concepts

#### 2.1 SQLite Basics

**What is SQLite?**
- Embedded database (no separate server)
- Single file on disk
- ACID compliant (Atomic, Consistent, Isolated, Durable)
- Perfect for desktop applications

**SQLite C API Functions You'll Use:**
- `sqlite3_open()` - Open database
- `sqlite3_close()` - Close database
- `sqlite3_prepare_v2()` - Prepare SQL statement
- `sqlite3_bind_*()` - Bind parameters (prevent SQL injection)
- `sqlite3_step()` - Execute statement
- `sqlite3_column_*()` - Read results
- `sqlite3_finalize()` - Clean up statement
- `sqlite3_exec()` - Execute simple SQL

#### 2.2 Database Schema Design

**Users Table:**
```sql
- id: Unique identifier (primary key)
- username: Login name (unique)
- master_hash: Hashed derived key (for verification)
- salt: Random salt for key derivation
- created_at: Timestamp
```

**Passwords Table:**
```sql
- id: Unique identifier (primary key)
- user_id: Foreign key to users table
- service: Service name (e.g., "gmail", "github")
- username: Username for that service
- encrypted_password: The encrypted password (BLOB)
- nonce: Nonce used for encryption (BLOB)
- url: Optional website URL
- notes: Optional notes
- created_at: Timestamp
- updated_at: Timestamp
```

#### 2.3 Why Use Prepared Statements?

**SQL Injection Example (DANGEROUS):**
```cpp
// NEVER DO THIS!
string sql = "SELECT * FROM users WHERE username = '" + username + "'";
// If username = "admin' OR '1'='1", you're hacked!
```

**Safe Way (Prepared Statements):**
```cpp
sqlite3_prepare_v2(db, "SELECT * FROM users WHERE username = ?", ...);
sqlite3_bind_text(stmt, 1, username.c_str(), ...);
// SQL injection is impossible - the parameter is treated as data
```

#### 2.4 RAII for Database Resources

**Problem:**
```cpp
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
// If exception occurs here, stmt is never finalized - memory leak!
sqlite3_step(stmt);
sqlite3_finalize(stmt);
```

**Solution: RAII Wrapper**
```cpp
Create a StatementWrapper class that:
- Wraps sqlite3_stmt*
- Automatically calls sqlite3_finalize() in destructor
- Cannot be copied (only moved)
```

### Implementation Tasks

#### Task 2.1: Define Data Structures (database.hpp)
- `struct User` - Represents a user record
- `struct PasswordEntry` - Represents a password record
- Include all fields from schema

#### Task 2.2: Create Database Class (database.hpp)

**Private members:**
- `sqlite3* db_` - Database connection handle
- Helper methods:
  - `executeSQL(sql)` - Execute simple SQL
  - `bindBlob(stmt, index, data)` - Bind binary data
  - `getColumnBlob(stmt, column)` - Read binary data

**Public methods:**
- Constructor: `Database(db_path)` - Open connection
- Destructor: `~Database()` - Close connection
- `initialize()` - Create tables if not exist
- User operations:
  - `createUser(username, master_hash, salt)`
  - `getUser(username)` - Returns `std::optional<User>`
  - `userExists(username)` - Returns `bool`
- Password operations:
  - `addPassword(entry)`
  - `getPassword(user_id, service)` - Returns `std::optional<PasswordEntry>`
  - `listPasswords(user_id)` - Returns `std::vector<PasswordEntry>`
  - `updatePassword(entry_id, encrypted_password, nonce)`
  - `deletePassword(entry_id)`
  - `passwordExists(user_id, service)` - Returns `bool`

#### Task 2.3: Implement Constructor/Destructor (database.cpp)

**Constructor:**
1. Call `sqlite3_open()` to open/create database file
2. Check return code
3. If error: throw exception with error message
4. Store `db_` pointer

**Destructor:**
1. Call `sqlite3_close()` to close connection
2. Set `db_` to nullptr
3. No exceptions in destructor!

#### Task 2.4: Implement initialize() (database.cpp)

**What to do:**
1. Create SQL for users table (see schema above)
2. Create SQL for passwords table (see schema above)
3. Call `executeSQL()` for each table
4. Add UNIQUE constraint on (user_id, service) in passwords table
5. Handle errors appropriately

#### Task 2.5: Implement User Operations (database.cpp)

**createUser():**
1. Prepare SQL: `INSERT INTO users (username, master_hash, salt) VALUES (?, ?, ?)`
2. Bind username as text
3. Bind master_hash as BLOB
4. Bind salt as BLOB
5. Execute with `sqlite3_step()`
6. Finalize statement
7. Check for errors (e.g., duplicate username)

**getUser():**
1. Prepare SQL: `SELECT id, username, master_hash, salt FROM users WHERE username = ?`
2. Bind username
3. Execute with `sqlite3_step()`
4. If `SQLITE_ROW`: read columns and create User object
5. If `SQLITE_DONE`: return `std::nullopt`
6. Finalize statement
7. Return `std::optional<User>`

#### Task 2.6: Implement Password Operations (database.cpp)

**addPassword():**
1. Prepare SQL: `INSERT INTO passwords (...) VALUES (?, ?, ?, ?, ?, ...)`
2. Bind all fields
3. Execute
4. Handle errors (duplicate service name for user)

**getPassword():**
1. Prepare SQL: `SELECT * FROM passwords WHERE user_id = ? AND service = ?`
2. Bind parameters
3. Execute and read row if exists
4. Return `std::optional<PasswordEntry>`

**listPasswords():**
1. Prepare SQL: `SELECT * FROM passwords WHERE user_id = ?`
2. Bind user_id
3. Loop with `sqlite3_step()` until `SQLITE_DONE`
4. For each row, create PasswordEntry and add to vector
5. Return vector

**updatePassword():**
1. Prepare SQL: `UPDATE passwords SET encrypted_password = ?, nonce = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?`
2. Bind parameters
3. Execute
4. Check rows affected (should be 1)

**deletePassword():**
1. Prepare SQL: `DELETE FROM passwords WHERE id = ?`
2. Bind id
3. Execute

#### Task 2.7: Implement Helper Methods (database.cpp)

**executeSQL():**
- Simple wrapper around `sqlite3_exec()`
- Throws exception on error

**bindBlob():**
- Wrapper around `sqlite3_bind_blob()`
- Takes vector<uint8_t>, binds to parameter

**getColumnBlob():**
- Gets BLOB column from result
- Uses `sqlite3_column_blob()` and `sqlite3_column_bytes()`
- Returns `vector<uint8_t>`

### Security Considerations

#### 2.8 Database Security
- Never store plaintext passwords (always encrypted)
- Never store master password (only hash)
- Use prepared statements (prevent SQL injection)
- Set proper file permissions on database file
- Consider encrypting the entire database file (advanced)

#### 2.9 Data Integrity
- Use transactions for multi-step operations
- Validate data before storing
- Check foreign key constraints
- Handle unique constraint violations gracefully

### Testing Plan

**Test each operation:**
1. Create database and initialize
2. Create user → verify it exists
3. Try to create duplicate user → should fail
4. Get user → verify all fields match
5. Add password → verify it's stored
6. Get password → verify decryption works
7. List passwords → verify all returned
8. Update password → verify changes saved
9. Delete password → verify it's gone
10. Test with multiple users

---

## Step 3: Password Manager Logic

### Goal
Create the PasswordManager class that orchestrates CryptoEngine and Database to provide high-level password management operations.

### Learning Objectives
- Design clean APIs
- Manage application state (logged in user)
- Coordinate multiple components
- Implement secure workflows

### Files to Create
- `cpp-core/include/password_manager.hpp` - Header with PasswordManager class
- `cpp-core/src/password_manager.cpp` - Implementation

### Key Concepts

#### 3.1 What Does PasswordManager Do?

**It's the "brain" that:**
- Manages user sessions (who's logged in?)
- Keeps the derived key in memory (while logged in)
- Coordinates crypto and database operations
- Enforces business rules (e.g., no duplicate services)
- Provides clean API for CLI to use

#### 3.2 Application State

**States:**
- **No User Loaded**: No operations allowed
- **User Loaded, Not Authenticated**: Limited operations
- **User Authenticated**: Full access to passwords

**Session Data (while logged in):**
- Current user ID
- Username
- Derived encryption key (in memory)
- Session timeout (optional security feature)

#### 3.3 The Workflow

**Registration:**
1. User provides username and master password
2. Generate salt
3. Derive key from password + salt
4. Hash key for storage
5. Store: username, hash, salt in database
6. Return success

**Login:**
1. User provides username and master password
2. Retrieve user from database (get salt)
3. Derive key from password + salt
4. Hash derived key
5. Compare with stored hash
6. If match: store key in memory, set authenticated state
7. Return success/failure

**Add Password:**
1. Check: is user authenticated?
2. User provides service name, username, password
3. Check: does password for this service already exist?
4. Encrypt password with derived key
5. Store: service, username, encrypted data, nonce
6. Return success

**Get Password:**
1. Check: is user authenticated?
2. User provides service name
3. Retrieve encrypted password + nonce from database
4. Decrypt with derived key
5. Return plaintext password

**Other Operations:**
- List all services
- Update existing password
- Delete password
- Generate and store password
- Logout (clear key from memory)

### Implementation Tasks

#### Task 3.1: Design PasswordManager Class (password_manager.hpp)

**Private members:**
- `std::unique_ptr<CryptoEngine> crypto_`
- `std::unique_ptr<Database> db_`
- `std::optional<int> current_user_id_`
- `std::string current_username_`
- `std::vector<uint8_t> session_key_` (the derived key)
- `bool authenticated_`

**Private methods:**
- `requireAuthentication()` - Throws if not authenticated
- `clearSession()` - Zeros session_key and resets state

**Public methods:**
- Constructor: `PasswordManager(db_path)`
- `registerUser(username, master_password)` → bool
- `login(username, master_password)` → bool
- `logout()` → void
- `isAuthenticated()` → bool
- `getCurrentUsername()` → string
- `addPassword(service, username, password, url, notes)` → bool
- `getPassword(service)` → optional<string>
- `listServices()` → vector<string>
- `updatePassword(service, new_password)` → bool
- `deletePassword(service)` → bool
- `generateAndStorePassword(service, username, length, flags)` → string
- `changePassword(service, old_password, new_password)` → bool

#### Task 3.2: Implement Constructor (password_manager.cpp)

**What to do:**
1. Create CryptoEngine instance
2. Create Database instance with provided path
3. Call `db_->initialize()` to create tables
4. Initialize state: authenticated = false

#### Task 3.3: Implement registerUser() (password_manager.cpp)

**Steps:**
1. Check if user already exists → return false
2. Generate salt using `crypto_->generateSalt()`
3. Derive key using `crypto_->deriveKey(password, salt)`
4. Hash key using `crypto_->hashKeyForStorage(key)`
5. Store user in database with `db_->createUser(username, hash, salt)`
6. Return true on success
7. Handle exceptions appropriately

#### Task 3.4: Implement login() (password_manager.cpp)

**Steps:**
1. Retrieve user from database with `db_->getUser(username)`
2. If user doesn't exist → return false
3. Derive key from password + stored salt
4. Hash the derived key
5. Compare hashed key with stored hash (use constant-time comparison!)
6. If match:
   - Store current_user_id
   - Store current_username
   - Store session_key (the derived key, NOT the hash!)
   - Set authenticated = true
   - Return true
7. If no match → return false

#### Task 3.5: Implement logout() (password_manager.cpp)

**Steps:**
1. Zero out session_key (security!)
2. Clear current_user_id
3. Clear current_username
4. Set authenticated = false

#### Task 3.6: Implement addPassword() (password_manager.cpp)

**Steps:**
1. Call `requireAuthentication()` → throws if not logged in
2. Check if password already exists for this service → return false or error
3. Encrypt the password:
   - Call `crypto_->encrypt(password, session_key)`
   - Get back: ciphertext and nonce
4. Create PasswordEntry struct with all data
5. Store in database with `db_->addPassword(entry)`
6. Return true on success

#### Task 3.7: Implement getPassword() (password_manager.cpp)

**Steps:**
1. Call `requireAuthentication()`
2. Retrieve password entry from database
3. If not found → return nullopt
4. Decrypt the password:
   - Call `crypto_->decrypt(ciphertext, session_key, nonce)`
5. Return decrypted password

#### Task 3.8: Implement Other Operations (password_manager.cpp)

**listServices():**
- Get all passwords for current user
- Extract service names
- Return vector of strings

**updatePassword():**
- Get existing entry
- Encrypt new password
- Update in database

**deletePassword():**
- Delete from database by service name

**generateAndStorePassword():**
- Generate password with crypto engine
- Store like addPassword()
- Return the generated password (so user can see it)

### Security Considerations

#### 3.9 Session Security
- **Key in Memory**: The session_key is sensitive! It's the encryption key!
- **Clear on Logout**: Always zero it out
- **Auto-Lock**: Consider implementing timeout (logout after inactivity)
- **No Logging**: Never log the session key or passwords

#### 3.10 Password Verification
- **Constant-Time Comparison**: When comparing hashes, use constant-time comparison
- **Rate Limiting**: Consider limiting login attempts (future enhancement)
- **No Hints**: Don't tell user if username or password was wrong (just "login failed")

### Testing Plan

**Test the workflows:**
1. Register new user → verify in database
2. Try to register duplicate → should fail
3. Login with correct password → should succeed
4. Login with wrong password → should fail
5. Try operations without login → should fail
6. Add password → verify it's stored encrypted
7. Get password → verify it's decrypted correctly
8. List services → verify all returned
9. Update password → verify changes work
10. Delete password → verify it's gone
11. Logout → verify can't access passwords
12. Login again → verify can access again

---

## Step 4: CLI Interface

### Goal
Create a user-friendly command-line interface for interacting with the password manager.

### Learning Objectives
- Command-line argument parsing
- Secure password input (hide characters)
- User experience design
- Input validation

### Files to Update
- `cpp-core/src/main.cpp` - Main CLI implementation

### Key Concepts

#### 4.1 CLI Commands

**Suggested command structure:**
```
cryptify init                                  # Initialize database
cryptify register                              # Register new user
cryptify login                                 # Login
cryptify add <service>                         # Add password
cryptify get <service>                         # Get password
cryptify list                                  # List all services
cryptify generate <service> [length]           # Generate & store
cryptify update <service>                      # Update password
cryptify delete <service>                      # Delete password
cryptify logout                                # Logout
cryptify help                                  # Show help
```

#### 4.2 Secure Password Input

**Problem:** `std::cin` shows what you type
**Solution:** Use platform-specific functions to hide input

**Windows:**
```cpp
Use SetConsoleMode to disable echo
Or use _getch() to read char by char
```

**Linux/Mac:**
```cpp
Use termios to disable echo
Or use getpass() function
```

#### 4.3 User Experience Considerations
- Clear prompts
- Helpful error messages
- Confirmation for destructive operations (delete)
- Show password strength when generating
- Clipboard copy option (advanced)
- Session persistence (stay logged in)

### Implementation Tasks

#### Task 4.1: Implement Secure Password Input

**Create helper function:**
```cpp
string getPasswordInput(const string& prompt);
```

**What it does:**
1. Display prompt
2. Disable terminal echo
3. Read password character by character
4. Don't display characters (or show asterisks)
5. Enable terminal echo again
6. Return password string

#### Task 4.2: Implement Command Parser

**Create function:**
```cpp
void handleCommand(const string& command, PasswordManager& pm);
```

**What it does:**
1. Parse command and arguments
2. Call appropriate PasswordManager method
3. Handle errors and display messages
4. Return to prompt

#### Task 4.3: Implement Each Command Handler

**register command:**
1. Prompt for username
2. Prompt for master password (hidden)
3. Prompt to confirm password
4. Check passwords match
5. Call `pm.registerUser()`
6. Display success or error message

**login command:**
1. Prompt for username
2. Prompt for master password (hidden)
3. Call `pm.login()`
4. Display success or error message

**add command:**
1. Check if logged in
2. Get service name from argument
3. Prompt for username for that service
4. Prompt for password (or offer to generate)
5. Optional: prompt for URL and notes
6. Call `pm.addPassword()`
7. Display success message

**get command:**
1. Check if logged in
2. Get service name from argument
3. Call `pm.getPassword()`
4. Display password (consider security: clear screen after?)
5. Optional: copy to clipboard

**list command:**
1. Check if logged in
2. Call `pm.listServices()`
3. Display services in a nice format

**generate command:**
1. Check if logged in
2. Get service name from argument
3. Get length from argument or prompt
4. Ask for password requirements (uppercase, numbers, symbols)
5. Call `pm.generateAndStorePassword()`
6. Display generated password

**update command:**
1. Check if logged in
2. Get service name
3. Prompt for new password
4. Call `pm.updatePassword()`
5. Display success message

**delete command:**
1. Check if logged in
2. Get service name
3. Confirm deletion (Are you sure?)
4. Call `pm.deletePassword()`
5. Display success message

**logout command:**
1. Call `pm.logout()`
2. Display logout message

**help command:**
1. Display all available commands with descriptions

#### Task 4.4: Implement Main Loop

**Create interactive mode:**
```cpp
while (running) {
    Display prompt (show username if logged in)
    Read command
    Parse command
    Execute command
    Handle errors
}
```

**Or single-command mode:**
```cpp
Parse arguments
Execute single command
Exit
```

#### Task 4.5: Add Input Validation

**Validate:**
- Username: alphanumeric, reasonable length
- Service name: not empty, no special characters
- Password length: minimum 8, maximum 128
- Master password: minimum strength requirements

### Security Considerations

#### 4.6 CLI Security
- **Clear Sensitive Data**: Clear password from memory after use
- **Screen Privacy**: Consider clearing screen after showing passwords
- **History**: Warn users to not pass passwords as command arguments (they'd be in shell history)
- **Auto-Lock**: Implement timeout (logout after inactivity)

#### 4.7 Error Messages
- Don't reveal too much information
- "Login failed" not "Username doesn't exist"
- Be helpful but security-conscious

### Testing Plan

**Test each command:**
1. Test help command
2. Test register with valid/invalid inputs
3. Test login with correct/wrong credentials
4. Test add password
5. Test get password
6. Test list (empty and with data)
7. Test generate password
8. Test update password
9. Test delete password
10. Test logout
11. Test commands when not logged in (should fail)
12. Test error handling (invalid service, etc.)

---

## Testing Strategy

### Unit Tests
**For each class:**
- Test each method individually
- Mock dependencies if needed
- Test normal cases and edge cases
- Test error conditions

### Integration Tests
**Full workflows:**
- Complete user registration → login → add → get → update → delete cycle
- Multiple users, multiple passwords
- Error recovery

### Security Tests
**Verify:**
- Passwords are encrypted in database
- Master password is not stored
- Session key is cleared on logout
- Memory is zeroed for sensitive data
- SQL injection is prevented

### Tools to Use
- **Google Test** or **Catch2** for unit tests
- **Valgrind** for memory leak detection
- **AddressSanitizer** for memory errors
- **SQLite Browser** to inspect database

---

## Common Issues

### Issue 1: Linker Errors
**Problem:** Can't find Rust library
**Solution:** 
- Check CMakeLists.txt path to Rust lib
- Ensure Rust lib is built (`cargo build --release`)
- Add correct system libraries for your platform

### Issue 2: SQLite Not Found
**Problem:** CMake can't find SQLite3
**Solution:**
- Install via vcpkg: `vcpkg install sqlite3:x64-windows`
- Or download precompiled binaries
- Update CMakeLists.txt with correct path

### Issue 3: Undefined Rust Symbols
**Problem:** Linker can't find Rust functions
**Solution:**
- Check function names match exactly (Rust and C++)
- Verify `#[no_mangle]` in Rust
- Check `extern "C"` in C++
- Rebuild Rust library

### Issue 4: Segmentation Fault at FFI Boundary
**Problem:** Crash when calling Rust
**Solution:**
- Validate all pointers before passing to Rust
- Ensure buffers are large enough
- Check pointer lifetimes
- Use debugger to find exact location

### Issue 5: SQLite Errors
**Problem:** Database operations fail
**Solution:**
- Check SQL syntax
- Verify table exists (call initialize())
- Check constraints (unique, foreign key)
- Use sqlite3_errmsg() for detailed errors

### Issue 6: Memory Leaks
**Problem:** Valgrind reports leaks
**Solution:**
- Ensure sqlite3_finalize() is called
- Use RAII wrappers
- Check for exceptions that skip cleanup
- Use smart pointers

---

## Implementation Timeline

### Week 1-2: Crypto Wrapper
- [ ] Create header and source files
- [ ] Implement all wrapper methods
- [ ] Write unit tests
- [ ] Test from main.cpp
- [ ] Fix any FFI issues

### Week 2-3: Database Layer
- [ ] Design schema
- [ ] Create Database class
- [ ] Implement user operations
- [ ] Implement password operations
- [ ] Test all CRUD operations

### Week 3-4: Password Manager
- [ ] Create PasswordManager class
- [ ] Implement registration/login
- [ ] Implement password operations
- [ ] Add session management
- [ ] Test all workflows

### Week 4-5: CLI Interface
- [ ] Implement secure password input
- [ ] Create command parser
- [ ] Implement all commands
- [ ] Add input validation
- [ ] Test user experience

### Week 5: Integration & Testing
- [ ] Full integration tests
- [ ] Security audit
- [ ] Memory leak checks
- [ ] Performance testing
- [ ] Bug fixes

---

## Success Criteria

**You're done when:**
- [ ] All classes compile without errors
- [ ] All unit tests pass
- [ ] Integration tests pass
- [ ] No memory leaks (Valgrind clean)
- [ ] Passwords are encrypted in database
- [ ] Master password is never stored
- [ ] CLI is user-friendly
- [ ] Error handling is robust
- [ ] Code is documented
- [ ] You understand every line of code

---

## Next Steps After C++ Implementation

Once C++ is complete:
1. **Phase 6**: Dockerization
2. **Phase 7**: Advanced features
3. **Phase 8**: GUI (optional)

But for now, focus on getting the C++ backend solid and working!

Good luck! 🚀

---

## Progress Tracker

### Week 1-2: Crypto Wrapper
- [ ] Study FFI concepts
- [ ] Create crypto_wrapper.hpp
- [ ] Declare Rust functions
- [ ] Define exception classes
- [ ] Implement CryptoEngine class
- [ ] Test each method
- [ ] Fix any issues

### Week 2-3: Database
- [ ] Study SQLite C API
- [ ] Design database schema
- [ ] Create database.hpp
- [ ] Implement Database class
- [ ] Test user operations
- [ ] Test password operations
- [ ] Verify data integrity

### Week 3-4: Password Manager
- [ ] Design PasswordManager API
- [ ] Create password_manager.hpp
- [ ] Implement registration/login
- [ ] Implement password operations
- [ ] Add session management
- [ ] Test all workflows
- [ ] Security review

### Week 4-5: CLI
- [ ] Implement secure input
- [ ] Create command parser
- [ ] Implement all commands
- [ ] Add validation
- [ ] Test user experience
- [ ] Polish and refine

### Week 5: Final Testing
- [ ] Integration tests
- [ ] Security audit
- [ ] Memory checks
- [ ] Performance testing
- [ ] Documentation
- [ ] Ready for Phase 6!
