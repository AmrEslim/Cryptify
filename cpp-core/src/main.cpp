#include <iostream>
#include <vector>
#include <iomanip>

extern "C" {
    int rust_generate_salt(uint8_t* salt_out, size_t salt_len);

}

int main() {
    std::cout << "Testing Salt Generation...\n";
    // Call the Rust function
   
    // Test 1: Generate a 16-byte salt
    std::vector<unsigned char> salt1(16);
    int result = rust_generate_salt(salt1.data(), salt1.size());
    
    std::cout << "Salt 1 (16 bytes): ";
    for (unsigned char byte : salt1) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << "\n";
    
    // Test 2: Generate another salt - should be different!
    std::vector<unsigned char> salt2(16);
    rust_generate_salt(salt2.data(), salt2.size());
    
    std::cout << "Salt 2 (16 bytes): ";
    for (unsigned char byte : salt2) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << "\n";
    
    // Test 3: Verify they're different
    bool same = (salt1 == salt2);
    std::cout << "\nSalts are " << (same ? "SAME (BAD!)" : "DIFFERENT (GOOD!)") << "\n";
    
    return 0;

}