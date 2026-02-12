#pragma once
#include <vector>
#include <iostream>
class CryptoManager
{
public:
    static std::vector<uint8_t> generateRandomBytes(int size);
    static std::vector<uint8_t> deriveKey(const std::string &pass, const std::vector<uint8_t> &salt);
    static std::vector<uint8_t> encrypt(const std::string &plaintext, const std::vector<uint8_t> &key, ...);

private:
};
