#include "CryptoManager.hpp"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/aes.h> 
#include <stdexcept>


//implemented 
std::vector<uint8_t> CryptoManager::generateRandomBytes(int size){
    std::vector<uint8_t> buffer(size);
    if(RAND_bytes(buffer.data(), size) != 1){
        throw std::runtime_error("Failed to generate random bytes");
    } 
    return buffer;
};


// still working on....

std::vector<uint8_t> CryptoManager::hashPassword(const std::string &password, const  std::vector<uint8_t> &salt){
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    // 2. Context setup
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    // 3. Initialize SHA256
    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Hash init failed");
    }
    // 4. Feed data (Password then Salt)
    EVP_DigestUpdate(context, password.c_str(), password.size());
    EVP_DigestUpdate(context, salt.data(), salt.size());
    // 5. Finalize
    unsigned int length = 0;
    EVP_DigestFinal_ex(context, hash.data(), &length);
    
    // 6. Cleanup
    EVP_MD_CTX_free(context);

    return hash;
};

std::vector<uint8_t> CryptoManager::deriveKey(const std::string &pass, const std::vector<uint8_t> &salt){
    const int ITERATIONS = 100000;
    const int KEY_LENGTH = 32; 
    std::vector<uint8_t> key(KEY_LENGTH);
// PKCS5_PBKDF2_HMAC is the OpenSSL function for this
    int result = PKCS5_PBKDF2_HMAC(
        pass.c_str(), pass.length(),
        salt.data(), salt.size(),
        ITERATIONS,
        EVP_sha256(),
        KEY_LENGTH,
        key.data()
    );
    if (result != 1) {
        throw std::runtime_error("Failed to derive key");
    }

    return key;
}


std::vector<uint8_t> CryptoManager::encrypt(const std::string& plaintext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    
    // 1. Context Setup
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create cipher context");

    // 2. Init Encryption (AES-256-GCM)
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx); 
        throw std::runtime_error("Encrypt init warning");
    }

    // 3. Set Key and IV
    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encrypt init failed");
    }

    // 4. Provide Data
    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH); // Allocate enough space
    int len;
    int ciphertext_len;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                          reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encrypt update failed");
    }
    ciphertext_len = len;

    // 5. Finalize
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encrypt final failed");
    }
    ciphertext_len += len;

    // 6. Get the Tag (Critical for GCM!)
    // The tag is usually 16 bytes. We verify this during decryption.
    std::vector<uint8_t> tag(16);
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to get GCM tag");
    }

    // 7. Cleaning up
    EVP_CIPHER_CTX_free(ctx);

    // 8. Combine Ciphertext + Tag
    // We resize to the actual length, then append the tag
    ciphertext.resize(ciphertext_len);
    ciphertext.insert(ciphertext.end(), tag.begin(), tag.end());

    return ciphertext;
}

std::vector<uint8_t> CryptoManager::decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    
    // 1. Validation
    if (ciphertext.size() < 16) {
        throw std::runtime_error("Ciphertext too short (no tag)");
    }

    // 2. Separate Tag from Encrypted Data
    // The tag is the Last 16 bytes
    std::vector<uint8_t> tag(ciphertext.end() - 16, ciphertext.end());
    std::vector<uint8_t> realCiphertext(ciphertext.begin(), ciphertext.end() - 16);

    // 3. Context Setup
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create cipher context");

    // 4. Init Decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx); 
        throw std::runtime_error("Decrypt init warning");
    }

    // 5. Set Key and IV
    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt init failed");
    }

    // 6. Feed Data
    // We resize buffer to same size as input (plaintext is usually same size or smaller)
    std::vector<uint8_t> plaintext(realCiphertext.size());
    int len;
    int plaintext_len;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                          realCiphertext.data(), realCiphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt update failed");
    }
    plaintext_len = len;

    // 7. SET THE TAG (Critical for Verification)
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to set expected tag");
    }

    // 8. Finalize (Checks the Tag)
    // If this returns <= 0, it means Auth Failed (Wrong Key or Tampered Data)
    int ret = EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        plaintext_len += len;
        plaintext.resize(plaintext_len);  
    return plaintext;
    } else {
        throw std::runtime_error("Decryption Verification Failed! Wrong Key or Corrupted Data.");
    }
}
