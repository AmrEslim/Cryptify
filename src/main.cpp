#include <iostream>
#include <sqlite3.h>
#include"dBase.hpp"
#include "CryptoManager.hpp"
#include <limits> // tinkering with this later .....
#include "CLI.hpp"



// int LoggedInUser{-1};
// std::vector<uint8_t> currentMasterKey;



int main() {
    auto title = "starting Cryptify...";
    CLI::printBanner(title);
    dataBase db{"cryptify.db"}; 
    // CLI::clearScreen();
    std::string username = CLI::getLine("enter new username :");
    std::string password = CLI::getLine("enter new password :");

    auto salt = CryptoManager::generateRandomBytes(16);
    auto passHash = CryptoManager::hashPassword(password, salt);

    if(db.addUser(username, passHash, salt)){
        std::cout << "user created successfully \n";
    }else {
        std::cout << "user creation failed  \n";
    }
    std::cout << "try to login now \n" << "enter user name and pass \n";
    username = "";
    password= "";
    dataBase::UserQuerey outData;
    std::cin >> username;
    std::cin >> password;
    db.getUser(username, outData);
    passHash = CryptoManager::hashPassword(password, outData.salt);
    std::vector<uint8_t> currentMasterKey;
    if (passHash == outData.hash){
        std::cout << "login successfull welcome back  \n";
        currentMasterKey = CryptoManager::deriveKey(password, salt);

    }else {
        std::cout << "user login failed  \n";
    }


    return 0;

}






    // try {
    //     dataBase db{"cryptify.db"}; 
        
    //     // 1. Simulate a User Login / Key Generation
    //     std::string userPassword = "MySuperSecretPassword123!";
    //     // In real app, you get this salt from the DB using `getUser`
    //     auto masterSalt = CryptoManager::generateRandomBytes(16); 
        
    //     std::cout << "Deriving key...\n";
    //     auto masterKey = CryptoManager::deriveKey(userPassword, masterSalt);

    //     // 2. Encrypt a Secret
    //     std::string secretUrl = "facebook.com";
    //     std::string secretPass = "Password123"; // This is what we want to hide

    //     auto iv = CryptoManager::generateRandomBytes(12); // GCM standard IV is 12 bytes
    //     auto encryptedPass = CryptoManager::encrypt(secretPass, masterKey, iv);

    //     // // 3. Store in DB
    //     // db.addSecret(1, secretUrl, encryptedPass, iv);
        
    //     // std::cout << "Secret encrypted and saved! Size: " << encryptedPass.size() << " bytes.\n";
    //     // 4. Test Retrieval
    //     std::cout << "\n--- Testing Retrieval ---\n";
    //     auto secrets = db.getSecrets(1); // Get secrets for User ID 1

    //     if (secrets.empty()) {
    //         std::cout << "No secrets found in DB!\n";
    //     } else {
    //         std::cout << "Found " << secrets.size() << " secrets.\n"; 
            
    //         // Loop through them and try to decrypt
    //        for (const auto& record : secrets) {
    //             std::cout << "Decryping secret for: " << record.title << "...\n";

    //             try {
    //                 auto recoveredBytes = CryptoManager::decrypt(record.encryptedData, masterKey, record.iv);
    //                 std::string recoveredPass(recoveredBytes.begin(), recoveredBytes.end());
    //                 std::cout << "SUCCESS! Password is: " << recoveredPass << "\n";
    //             } catch (const std::exception& e) {
    //                 std::cout << "FAILED: " << e.what() << "\n";
    //             }
    //         }
    //     }

    // } catch(const std::exception& e) {
    //     std::cout << "ERROR: " << e.what() << "\n";
    // }