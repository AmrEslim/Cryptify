#pragma once
#include <vector>
#include <cstdint>
#include <string>
class CryptoManager
{
public:
    static std::vector<uint8_t> generateRandomBytes(int size);
    static std::vector<uint8_t> hashPassword(const std::string &password, const std::vector<uint8_t> &salt);
    static std::vector<uint8_t> deriveKey(const std::string &pass, const std::vector<uint8_t> &salt);
    static std::vector<uint8_t> encrypt(const std::string &plaintext, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
    static std::vector<uint8_t> decrypt(const std::vector<uint8_t> &cipherText, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);

private:
};
