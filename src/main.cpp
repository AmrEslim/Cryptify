#include <iostream>
#include <openssl/evp.h>
#include <sqlite3.h>
#include"dBase.hpp"


int main() {
    std::cout << "Testing Cryptify...\n";
    
    try {
        // This will create a file named "cryptify.db" in your build folder
        dataBase db{"cryptify.db"}; 
        
        // If we reach here, no exception was thrown
        std::cout << "App is running with DB connected!\n";
    } 
    catch(const std::exception& e) {
        std::cout << "CRITICAL ERROR: " << e.what() << std::endl;
        return 1; 
    }
    
    // Destructor runs here automatically
    return 0;

}