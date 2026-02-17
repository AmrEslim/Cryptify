cmake_minimum_required(VERSION 3.15)
project(Cryptify VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

# Simple test executable
add_executable(cryptify_test
    src/main.cpp
    src/dBase.cpp
    src/CryptoManager.cpp
    src/CLI.cpp
)
# Link imported targets (this automatically handles include paths and linking)
target_link_libraries(cryptify_test PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
    SQLite::SQLite3
)
# Tell the compiler where local headers are located
target_include_directories(cryptify_test PRIVATE include stdc++exp)

# Windows specific libraries needed for Rust
if(WIN32)
    target_link_libraries(cryptify_test PRIVATE 
        ws2_32 userenv bcrypt ntdll advapi32 Crypt32
    )
endif()

# Linux specific logic (optional, if you need libraries like pthread or dl)
if(UNIX AND NOT APPLE)
    target_link_libraries(cryptify_test PRIVATE
        pthread dl
    )
endif()