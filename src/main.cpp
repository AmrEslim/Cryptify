#include <iostream>
#include <openssl/evp.h>
#include <sqlite3.h>
#include"dBase.hpp"


int main() {
    std::cout << "Testing Cryptify...\n";

    try {
        // This will create a file named "cryptify.db" in your build folder
        dataBase db{"cryptify.db"}; 
        if (db.addUser("amor", "f99fdf8sdf9sdg9f89s8g9sdv98f9gs", "sdfsdfsdfsadvcfgewfwev32wef")){
            std::cout << "added user successfully...\n";
        }else{
            std::cout << "could not add user...\n";
        }
        // if (db.PrintUser("amor")){
        //     std::cout << "found user...\n";
        // }else{
        //     std::cout << "could not find user...\n";
        // }
        // db.PrintUser("amor");
        // dataBase::UserQuerey outData;
        // db.getUser("amor", outData);
        // std::cout << "user id : "<< outData.id << "\n" ;
        // std::cout << "user salt : ";
        // for(auto c : outData.salt) std::cout << c;
        // std::cout << "\n";
        // std::cout << "user hash : ";
        // for(auto c : outData.hash) std::cout << c;
        // std::cout << "\n";
        
        // If we reach here, no exception was thrown
        std::cout << "App is running with DB connected!\n";
        std::vector<uint8_t> dummyData = {0xDE, 0xAD, 0xBE, 0xEF};
        std::vector<uint8_t> dummyIv = {0x00, 0x01};
        if (db.addSecret(1, "facebook.com", dummyData, dummyIv)){
            std::cout << "added secret successfully...\n";
        }else{
            std::cout << "could not add secret...\n";
        }
        db.getSecrets(1);
        
    } 
    catch(const std::exception& e) {
        std::cout << "CRITICAL ERROR: " << e.what() << std::endl;
        return 1; 
    }
    

    
    
    // Destructor runs here automatically
    return 0;

}