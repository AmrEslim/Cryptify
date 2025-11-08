# Building a Dockerized Password Manager with Modern C++ and Rust

## Project Overview
This guide will walk you through building a secure password manager that leverages both Modern C++ and Rust, containerized with Docker. This project will help you learn cryptography, systems programming, memory safety, and containerization.

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Architecture Overview](#architecture-overview)
3. [Phase 1: Project Setup](#phase-1-project-setup)
4. [Phase 2: Rust Core - Cryptography Layer](#phase-2-rust-core---cryptography-layer)
5. [Phase 3: C++ Backend - Database & Business Logic](#phase-3-c-backend---database--business-logic)
6. [Phase 4: API Layer](#phase-4-api-layer)
7. [Phase 5: CLI Interface](#phase-5-cli-interface)
8. [Phase 6: Dockerization](#phase-6-dockerization)
9. [Phase 7: Testing & Security](#phase-7-testing--security)
10. [Phase 8: Advanced Features](#phase-8-advanced-features)
11. [Learning Resources](#learning-resources)

---

## Prerequisites

### Knowledge Requirements
- Basic understanding of C++ (C++17 or later)
- Basic understanding of Rust
- Familiarity with Git
- Basic knowledge of Docker
- Understanding of basic cryptography concepts

### Tools to Install
- **Rust**: Install via rustup (https://rustup.rs/)
- **C++ Compiler**: 
  - Linux: GCC 9+ or Clang 10+
  - Windows: MSVC 2019+ or MinGW-w64
  - macOS: Xcode Command Line Tools
- **CMake**: Version 3.15+
- **Docker**: Docker Desktop or Docker Engine
- **Git**: For version control
- **Text Editor/IDE**: VS Code, CLion, or your preferred IDE "VIM? ;)"

---

## Architecture Overview

### Why C++ and Rust Together?
- **Rust**: Handle cryptography (memory-safe, excellent crypto libraries)
- **C++**: Business logic, database operations (leverage existing libraries, performance)
- **Benefits**: Learn FFI (Foreign Function Interface), best practices from both languages

### High-Level Architecture
```
┌─────────────────────────────────────┐
│         CLI Interface (C++)         │
└─────────────┬───────────────────────┘
              │
┌─────────────▼───────────────────────┐
│    Business Logic Layer (C++)       │
│  - Password CRUD operations         │
│  - User authentication              │
│  - Database management              │
└─────────────┬───────────────────────┘
              │
┌─────────────▼───────────────────────┐
│   Cryptography Layer (Rust)         │
│  - Encryption/Decryption            │
│  - Key derivation (PBKDF2/Argon2)   │
│  - Random generation                │
│  - Hashing                          │
└─────────────────────────────────────┘
              │
┌─────────────▼───────────────────────┐
│      Storage Layer (SQLite)         │
│  - Encrypted password storage       │
│  - User credentials                 │
└─────────────────────────────────────┘
```

---

## Phase 1: Project Setup

### Step 1.1: Initialize Project Structure
Create the following directory structure:
```
Cryptify/
├── rust-crypto/          # Rust cryptography library
│   ├── src/
│   │   └── lib.rs
│   ├── Cargo.toml
│   └── build.rs
├── cpp-core/             # C++ application
│   ├── src/
│   │   ├── main.cpp
│   │   ├── crypto_wrapper.hpp
│   │   ├── crypto_wrapper.cpp
│   │   ├── password_manager.hpp
│   │   ├── password_manager.cpp
│   │   ├── database.hpp
│   │   └── database.cpp
│   ├── include/
│   ├── tests/
│   └── CMakeLists.txt
├── docker/
│   ├── Dockerfile
│   └── docker-compose.yml
├── tests/
├── docs/
├── README.md
└── .gitignore
```

### Step 1.2: Initialize Rust Project
```bash
cd rust-crypto
cargo init --lib
```

### Step 1.3: Initialize C++ Project
Create a basic `CMakeLists.txt` for the C++ project.

### Step 1.4: Set Up Git
Create `.gitignore` to exclude build artifacts, target directories, and sensitive files.

---

## Phase 2: Rust Core - Cryptography Layer

### Learning Goals
- Rust FFI (Foreign Function Interface)
- Cryptographic primitives
- Memory safety in crypto operations
- Error handling across language boundaries

### Step 2.1: Set Up Dependencies
In `rust-crypto/Cargo.toml`, add:
```toml
[dependencies]
aes-gcm = "0.10"           # AES-256-GCM encryption
argon2 = "0.5"             # Key derivation
rand = "0.8"               # Random number generation
zeroize = "1.7"            # Secure memory wiping
base64 = "0.21"            # Encoding

[lib]
crate-type = ["cdylib", "staticlib"]  # For FFI
```

### Step 2.2: Implement Core Crypto Functions
Create these functions in `lib.rs`:

1. **Key Derivation**: Derive encryption key from master password
   - Use Argon2id (resistant to GPU/ASIC attacks)
   - Learn about: salt, iterations, memory cost

2. **Encryption/Decryption**: 
   - Use AES-256-GCM (authenticated encryption)
   - Learn about: nonces, authenticated data, tags

3. **Random Generation**: 
   - Generate secure passwords
   - Generate salts and nonces

4. **Hashing**:
   - Hash master password for verification
   - Learn about: password hashing vs. encryption

### Step 2.3: Create C-Compatible FFI Interface
- Use `#[no_mangle]` and `extern "C"` for functions
- Convert Rust types to C-compatible types (pointers, integers)
- Handle errors properly (return error codes)
- Learn about: memory ownership across FFI boundary

### Step 2.4: Key Functions to Implement
```rust
// Examples of function signatures (implement these):
#[no_mangle]
pub extern "C" fn derive_key(password: *const c_char, ...) -> i32;

#[no_mangle]
pub extern "C" fn encrypt_password(plaintext: *const u8, ...) -> i32;

#[no_mangle]
pub extern "C" fn decrypt_password(ciphertext: *const u8, ...) -> i32;

#[no_mangle]
pub extern "C" fn generate_password(length: usize, ...) -> i32;
```

### Step 2.5: Build and Test
- Build: `cargo build --release`
- Create unit tests for each crypto function
- Test memory safety with Valgrind or Miri

---

## Phase 3: C++ Backend - Database & Business Logic

### Learning Goals
- Modern C++ features (smart pointers, RAII, std::optional)
- SQLite integration
- Calling Rust from C++
- Resource management

### Step 3.1: Set Up CMake
Configure CMake to:
- Link the Rust library
- Find SQLite3
- Set C++17/20 standard
- Enable warnings and sanitizers

### Step 3.2: Create Rust FFI Wrapper (C++)
In `crypto_wrapper.hpp` and `crypto_wrapper.cpp`:
- Declare the Rust functions (extern "C")
- Create C++ wrapper classes for type safety
- Use RAII for resource management
- Handle errors with exceptions or std::expected

Example structure:
```cpp
class CryptoEngine {
public:
    std::vector<uint8_t> encrypt(const std::string& plaintext, const std::string& key);
    std::string decrypt(const std::vector<uint8_t>& ciphertext, const std::string& key);
    std::string generatePassword(size_t length, bool includeSpecial);
    // ... other methods
private:
    // Wrap Rust functions
};
```

### Step 3.3: Implement Database Layer
In `database.hpp` and `database.cpp`:

1. **Database Schema**:
   ```sql
   CREATE TABLE users (
       id INTEGER PRIMARY KEY,
       username TEXT UNIQUE NOT NULL,
       master_hash TEXT NOT NULL,
       salt TEXT NOT NULL,
       created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
   );

   CREATE TABLE passwords (
       id INTEGER PRIMARY KEY,
       user_id INTEGER,
       service TEXT NOT NULL,
       username TEXT NOT NULL,
       encrypted_password BLOB NOT NULL,
       nonce BLOB NOT NULL,
       url TEXT,
       notes TEXT,
       created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
       updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
       FOREIGN KEY(user_id) REFERENCES users(id)
   );
   ```

2. **Database Class**:
   - Connection management
   - CRUD operations for passwords
   - User authentication
   - Use prepared statements (prevent SQL injection)
   - Learn about: RAII for database connections

### Step 3.4: Implement Password Manager Logic
In `password_manager.hpp` and `password_manager.cpp`:

Create a `PasswordManager` class with methods:
- `createUser(username, masterPassword)`
- `authenticateUser(username, masterPassword)`
- `addPassword(service, username, password)`
- `getPassword(service)`
- `updatePassword(service, newPassword)`
- `deletePassword(service)`
- `listPasswords()`
- `generateAndStorePassword(service, username, length)`

### Step 3.5: Implement Security Features
- Auto-lock after inactivity
- Clipboard management (clear after timeout)
- Master password verification
- Session management

---

## Phase 4: API Layer

### Learning Goals
- Separation of concerns
- API design
- Error handling strategies

### Step 4.1: Define Clear Interfaces
Create clean interfaces between components:
- Database interface
- Crypto interface
- Manager interface

### Step 4.2: Error Handling Strategy
Implement consistent error handling:
- Use custom exception classes or error codes
- Provide meaningful error messages
- Log errors appropriately

---

## Phase 5: CLI Interface

### Learning Goals
- Command-line parsing
- User input handling (secure password input)
- Terminal UI basics

### Step 5.1: Implement CLI
In `main.cpp`:

1. **Command Structure**:
   ```
   cryptify init                          # Initialize database
   cryptify register <username>           # Register new user
   cryptify login <username>              # Login
   cryptify add <service>                 # Add password
   cryptify get <service>                 # Get password
   cryptify list                          # List all services
   cryptify generate <service> [length]   # Generate & store
   cryptify update <service>              # Update password
   cryptify delete <service>              # Delete password
   cryptify export [file]                 # Export (encrypted)
   cryptify import <file>                 # Import
   ```

2. **Libraries to Consider**:
   - CLI11 or cxxopts for command-line parsing
   - ftxui or termcolor for terminal UI
   - getpass or similar for secure password input

### Step 5.2: Security Considerations
- Hide password input (use getpass-style input)
- Clear sensitive data from memory after use
- Lock after period of inactivity
- Warn on weak passwords

---

## Phase 6: Dockerization

### Learning Goals
- Multi-stage Docker builds
- Container security
- Build optimization

### Step 6.1: Create Dockerfile
In `docker/Dockerfile`:

```dockerfile
# Multi-stage build example structure:
# Stage 1: Build Rust library
FROM rust:1.75 as rust-builder
# ... build rust-crypto

# Stage 2: Build C++ application
FROM gcc:13 as cpp-builder
# ... copy rust library, build C++ app

# Stage 3: Runtime
FROM debian:bookworm-slim
# ... copy only necessary binaries
```

### Step 6.2: Optimize Docker Image
- Use multi-stage builds
- Minimize layers
- Use .dockerignore
- Don't include build tools in final image

### Step 6.3: Create docker-compose.yml
For easier management:
```yaml
version: '3.8'
services:
  cryptify:
    build:
      context: .
      dockerfile: docker/Dockerfile
    volumes:
      - cryptify-data:/data
    environment:
      - CRYPTIFY_DB_PATH=/data/passwords.db
volumes:
  cryptify-data:
```

### Step 6.4: Volume Management
- Store database in Docker volume
- Ensure data persistence
- Backup strategies

---

## Phase 7: Testing & Security

### Learning Goals
- Unit testing
- Integration testing
- Security best practices
- Vulnerability assessment

### Step 7.1: Rust Testing
In `rust-crypto/`:
- Write unit tests for each crypto function
- Test edge cases (empty passwords, max length, etc.)
- Test error handling
- Use `cargo test`

### Step 7.2: C++ Testing
- Use Google Test or Catch2
- Test each component in isolation
- Test FFI integration
- Test database operations
- Mock dependencies where appropriate

### Step 7.3: Integration Testing
- Test full workflows (register → login → add → get)
- Test error scenarios
- Test concurrent access (if applicable)

### Step 7.4: Security Checklist
- [ ] All passwords encrypted at rest
- [ ] Master password never stored (only hash)
- [ ] Secure key derivation (Argon2)
- [ ] Memory cleared after use (zeroize in Rust, explicit_bzero in C++)
- [ ] SQL injection prevention (prepared statements)
- [ ] Buffer overflow prevention
- [ ] No sensitive data in logs
- [ ] Secure random number generation
- [ ] Authenticated encryption (GCM mode)
- [ ] Protection against timing attacks

### Step 7.5: Security Testing Tools
- Valgrind (memory leaks)
- AddressSanitizer (buffer overflows)
- UndefinedBehaviorSanitizer
- Static analysis (clang-tidy, cppcheck)
- Cargo clippy for Rust

---

## Phase 8: Advanced Features

Once the core is solid, consider adding:

### Step 8.1: Password Strength Analyzer
- Check against common passwords
- Entropy calculation
- Pattern detection

### Step 8.2: Two-Factor Authentication
- TOTP generation
- Backup codes

### Step 8.3: Secure Notes
- Extend to store secure notes, not just passwords

### Step 8.4: Browser Extension
- Learn about: browser extension APIs
- Secure communication with CLI tool

### Step 8.5: Sync Capability
- Encrypted cloud sync
- Conflict resolution
- End-to-end encryption

### Step 8.6: GUI
- Qt or GTK for desktop GUI
- Web UI with Rust backend (Actix/Rocket)

### Step 8.7: Password Sharing
- Share specific passwords securely
- Public key cryptography

### Step 8.8: Audit Log
- Track all access to passwords
- Detect suspicious activity

---

## Learning Resources

### Rust
- **Official Book**: https://doc.rust-lang.org/book/
- **Rustlings**: Interactive exercises
- **Cryptography in Rust**: RustCrypto documentation
- **FFI Guide**: https://doc.rust-lang.org/nomicon/ffi.html

### Modern C++
- **C++ Core Guidelines**: https://isocpp.github.io/CppCoreGuidelines/
- **Effective Modern C++** by Scott Meyers
- **CPPReference**: https://en.cppreference.com/

### Cryptography
- **"Serious Cryptography"** by Jean-Philippe Aumasson
- **OWASP Password Storage Cheat Sheet**
- **Cryptography Engineering** by Ferguson, Schneier, Kohno
- **NaCl/libsodium documentation** (good practices)

### Docker
- **Docker Documentation**: https://docs.docker.com/
- **Docker Best Practices**: Official guide
- **Multi-stage builds tutorial**

### Security
- **OWASP Top 10**
- **CWE (Common Weakness Enumeration)**
- **Secure Coding in C++**

---

## Development Tips

### 1. Start Simple
- Begin with basic encryption/decryption
- Add features incrementally
- Test thoroughly at each stage

### 2. Version Control
- Commit often with clear messages
- Use branches for new features
- Tag releases

### 3. Documentation
- Document your code
- Keep this guide updated with your learnings
- Write a developer README

### 4. Code Review
- Review your own code after a day
- Ask others to review (if possible)
- Use linters and formatters

### 5. Performance
- Profile before optimizing
- Focus on correctness first
- Optimize hot paths only

### 6. Debugging
- Use debuggers (gdb, lldb, rust-gdb)
- Print/log strategically
- Reproduce issues reliably

---

## Milestones & Timeline Suggestion

### Week 1-2: Foundation
- [ ] Set up development environment
- [ ] Create project structure
- [ ] Basic Rust crypto functions
- [ ] FFI proof of concept

### Week 3-4: Core Functionality
- [ ] Complete Rust crypto library
- [ ] C++ wrapper for crypto
- [ ] Database schema and operations
- [ ] Basic password manager logic

### Week 5-6: Interface
- [ ] CLI implementation
- [ ] User registration/authentication
- [ ] CRUD operations
- [ ] Password generation

### Week 7-8: Containerization & Testing
- [ ] Docker setup
- [ ] Unit tests
- [ ] Integration tests
- [ ] Security audit

### Week 9+: Polish & Advanced Features
- [ ] Documentation
- [ ] Bug fixes
- [ ] Advanced features
- [ ] Performance optimization

---

## Common Pitfalls to Avoid

1. **Don't roll your own crypto**: Use established libraries
2. **Memory management**: Especially at FFI boundary
3. **Error handling**: Don't ignore errors, especially in crypto
4. **SQL injection**: Always use prepared statements
5. **Logging sensitive data**: Never log passwords or keys
6. **Weak key derivation**: Use proper parameters for Argon2
7. **Nonce reuse**: Always use unique nonces for encryption
8. **Not clearing memory**: Sensitive data should be zeroed

---

## Questions to Consider

As you build, think about:
- How do you handle the master password? (never store it!)
- What happens if the user forgets the master password?
- How do you prevent brute force attacks on the master password?
- How do you handle database migrations?
- What's your backup and recovery strategy?
- How do you detect corrupted data?
- What's the threat model? (local attacker, malware, etc.)

---

## Success Criteria

Your project is successful when:
- [ ] Passwords are securely encrypted
- [ ] Master password cannot be recovered or bypassed
- [ ] No memory leaks (verified with tools)
- [ ] All tests pass
- [ ] Security checklist complete
- [ ] Runs in Docker
- [ ] Code is clean and documented
- [ ] You understand every line of code you wrote

---

## Next Steps

1. **Read this guide thoroughly**
2. **Set up your development environment**
3. **Start with Phase 1**
4. **Work through each phase sequentially**
5. **Don't rush - understand each concept**
6. **Test continuously**
7. **Iterate and improve**

Remember: This is a learning project. The goal is not just to build a password manager, but to deeply understand systems programming, cryptography, FFI, and containerization. Take your time, experiment, make mistakes, and learn from them.

Good luck! 🚀🔐

---

## Additional Notes Section

Use this section to track your own learnings, challenges, and solutions as you work through the project.

### My Learnings
- 

### Challenges Faced
- 

### Solutions Found
- 

### Ideas for Improvement
- 
