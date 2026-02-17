# Cryptify 🔐

Cryptify is a small C++ password manager learning project.
It currently focuses on a CLI flow, password management logic, and a local SQLite-backed data layer.

## Current Scope

- C++17 codebase built with CMake
- CLI interface for interacting with the app
- Password management/business logic
- Database layer for local storage

## Project Layout

```
Cryptify/
├── include/             # Public headers
├── src/                 # Implementation files
├── docs/                # Project and implementation guides
├── CMakeLists.txt       # Build configuration
└── CMakePresets.json    # Preset build settings
```

## Dependencies

- CMake (3.15+)
- C++ compiler with C++23 support
- OpenSSL
- SQLite3
- vcpkg (package manager)

## Build & Run

```bash
cmake --preset windows-vcpkg 
cmake --build build
.\build\cryptify_test.exe
```

On Windows, the executable will be generated as `build\cryptify_test.exe`.

## Docs

- [GUIDE.md](docs/GUIDE.md)
- [CPP_IMPLEMENTATION.md](docs/CPP_IMPLEMENTATION.md)

## License

MIT — see [LICENSE](LICENSE).
