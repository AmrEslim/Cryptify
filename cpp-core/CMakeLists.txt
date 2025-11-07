cmake_minimum_required(VERSION 3.15)
project(Cryptify VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Simple test executable
add_executable(cryptify_test
    src/main.cpp
)

# Link the Rust library
target_link_libraries(cryptify_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../rust-crypto/target/release/rust_crypto.lib
)

# Windows specific libraries needed for Rust
if(WIN32)
    target_link_libraries(cryptify_test PRIVATE 
        ws2_32 userenv bcrypt ntdll advapi32
    )
endif()