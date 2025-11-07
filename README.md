# Cryptify 🔐

A secure, dockerized password manager built with **Modern C++** and **Rust** for learning systems programming, cryptography, and containerization.

## Features

- 🔒 **Military-grade encryption** - AES-256-GCM authenticated encryption
- 🛡️ **Secure key derivation** - Argon2id (resistant to GPU/ASIC attacks)
- 🦀 **Rust crypto core** - Memory-safe cryptographic operations
- ⚡ **C++ backend** - Fast database operations and business logic
- 🐳 **Fully containerized** - Runs anywhere with Docker
- 🔑 **Strong password generation** - Customizable random passwords
- 💾 **Local storage** - SQLite database with encrypted passwords

## Architecture

```
┌─────────────────┐
│   CLI (C++)     │  ← Command-line interface
└────────┬────────┘
         │
┌────────▼────────┐
│ Password        │  ← Business logic
│ Manager (C++)   │
└────┬───────┬────┘
     │       │
┌────▼────┐ ┌▼─────────┐
│Database │ │Crypto    │
│(SQLite) │ │(Rust FFI)│
└─────────┘ └──────────┘
```

## Project Structure

```
Cryptify/
├── rust-crypto/          # Rust cryptography library (FFI)
│   ├── src/lib.rs       # Crypto functions with detailed TODOs
│   └── Cargo.toml       # Rust dependencies
├── cpp-core/            # C++ application backend
│   ├── include/         # Header files with implementation guides
│   ├── src/             # Implementation files with TODO comments
│   └── CMakeLists.txt   # Build configuration
├── docker/              # Docker configuration (coming soon)
├── GUIDE.md            # Complete step-by-step learning guide
├── Rust_implementation.md       # Phase 2: Rust crypto implementation guide
└── CPP_IMPLEMENTATION.md  # Phase 3: C++ backend implementation guide
```

## Getting Started

### Prerequisites

- **Rust** (1.70+) - Install from [rustup.rs](https://rustup.rs/)
- **C++ Compiler** (C++17+) - MSVC 2019+, GCC 9+, or Clang 10+
- **CMake** (3.15+)
- **SQLite3**
- **Docker** (optional, for containerization)

### Building

**1. Build the Rust crypto library:**
```bash
cd rust-crypto
cargo build --release
```

**2. Build the C++ application:**
```bash
cd cpp-core
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

**3. Run:**
```bash
./cryptify_test  # or .\Release\cryptify_test.exe on Windows
```

## Learning Path

This is a **learning project** designed to teach modern systems programming concepts:

### ✅ **Phase 1: Setup** (Week 1)
- Development environment setup
- Project structure initialization
- FFI proof-of-concept (C++ ↔ Rust)

### 🎯 **Phase 2: Rust Cryptography** (Weeks 2-5)
- Random generation (salts, nonces, passwords)
- Key derivation with Argon2id
- AES-256-GCM encryption/decryption
- FFI wrappers for C++ integration
- 📖 See [`NEXT_STEPS.md`](NEXT_STEPS.md)

### 🎯 **Phase 3: C++ Backend** (Weeks 6-10)
- Crypto wrapper (safe C++ interface to Rust)
- Database layer (SQLite with prepared statements)
- Password manager logic (session management)
- CLI interface (secure password input)
- 📖 See [`CPP_IMPLEMENTATION.md`](CPP_IMPLEMENTATION.md)

### 🎯 **Phase 4: Dockerization** (Week 11)
- Multi-stage Docker builds
- Container optimization
- Volume management for data persistence

### 🎯 **Phase 5: Advanced Features** (Future)
- Password strength analyzer
- Two-factor authentication (TOTP)
- Secure notes
- GUI (Qt/GTK)

## Documentation

- **[GUIDE.md](/docs/GUIDE.md)** - Complete project overview and learning roadmap
- **[Rust_implementation.md](/docs/Rust_implementation.md)** - Detailed Rust implementation guide
- **[CPP_IMPLEMENTATION.md](/docs/CPP_IMPLEMENTATION.md)** - Detailed C++ implementation guide

Each file contains comprehensive TODO comments and step-by-step instructions.

## Security Features

- ✅ Master password never stored (only hash)
- ✅ Argon2id key derivation (memory-hard)
- ✅ AES-256-GCM authenticated encryption
- ✅ Unique nonce per encryption
- ✅ Prepared statements (SQL injection prevention)
- ✅ Memory zeroing for sensitive data
- ✅ No plaintext passwords in database

## Technology Stack

| Component | Technology | Purpose |
|-----------|-----------|---------|
| Cryptography | **Rust** | Memory-safe crypto operations |
| Backend | **C++17** | Database and business logic |
| Database | **SQLite3** | Local encrypted storage |
| Build System | **CMake** | Cross-platform builds |
| Container | **Docker** | Deployment and isolation |

## Learning Outcomes

By building this project, you'll learn:

- 🦀 Rust programming and memory safety
- ⚡ Modern C++ (RAII, smart pointers, templates)
- 🔗 FFI (Foreign Function Interface)
- 🔐 Applied cryptography (not rolling your own!)
- 💾 Database programming with SQLite
- 🐳 Docker containerization
- 🧪 Testing and debugging C++/Rust
- 🛡️ Secure software development practices

## Contributing

This is a learning project. Feel free to:
- 🐛 Report issues
- 💡 Suggest improvements
- 📚 Improve documentation
- 🎓 Share your learning experience

## License

MIT License - See [LICENSE](LICENSE) file for details.

## Disclaimer

⚠️ **Educational Purpose Only**  
This project is designed for learning. For production use, consider established password managers like:
- [Bitwarden](https://bitwarden.com/) (open-source)
- [KeePassXC](https://keepassxc.org/) (open-source)
- [1Password](https://1password.com/)

## Acknowledgments

Built as a learning project to explore:
- Systems programming concepts
- Cryptography best practices
- Language interoperability
- Secure software design

---

**Status:** 🚧 Under Development (Learning Project)

**Current Phase:** Phase 2 - Implementing Rust cryptography layer
