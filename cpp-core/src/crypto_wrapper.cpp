// ============================================================================
// CRYPTIFY - Crypto Wrapper Implementation
// ============================================================================
// This file implements the C++ wrapper around Rust cryptographic functions
// ============================================================================

#include "crypto_wrapper.hpp"
#include <cstring>
#include <algorithm>

// ============================================================================
// STEP 1: IMPLEMENT EXCEPTION CLASSES
// ============================================================================

// TODO: Implement CryptoException constructor
// Just forward the message to std::runtime_error
// Example: CryptoException::CryptoException(const std::string& message)
//          : std::runtime_error(message) {}

// TODO: Implement other exception constructors
// Follow the same pattern as CryptoException

// ============================================================================
// STEP 2: IMPLEMENT RANDOM GENERATION METHODS
// ============================================================================

std::vector<uint8_t> CryptoEngine::generateSalt() {
    // TODO: Implement salt generation
    //
    // Steps:
    // 1. Create a vector<uint8_t> with size SALT_SIZE
    //    std::vector<uint8_t> salt(SALT_SIZE);
    //
    // 2. Call the Rust function
    //    int result = rust_generate_salt(salt.data(), salt.size());
    //
    // 3. Check the result
    //    if (result != 0) {
    //        throw RandomGenerationError("Failed to generate salt");
    //    }
    //
    // 4. Return the salt
    //    return salt;
    
    throw std::runtime_error("Not implemented: generateSalt");
}

std::vector<uint8_t> CryptoEngine::generateNonce() {
    // TODO: Implement nonce generation
    //
    // Steps: Same as generateSalt but with NONCE_SIZE
    // 1. Create vector of size NONCE_SIZE
    // 2. Call rust_generate_nonce
    // 3. Check result and throw on error
    // 4. Return the nonce
    
    throw std::runtime_error("Not implemented: generateNonce");
}

std::string CryptoEngine::generatePassword(
    size_t length,
    bool includeUppercase,
    bool includeNumbers,
    bool includeSpecial)
{
    // TODO: Implement password generation
    //
    // Steps:
    // 1. Validate length
    //    if (length < 8 || length > 128) {
    //        throw std::invalid_argument("Invalid password length");
    //    }
    //
    // 2. Create buffer (length + 1 for null terminator)
    //    std::string password(length + 1, '\0');
    //
    // 3. Call Rust function
    //    int result = rust_generate_password(
    //        length,
    //        includeUppercase,
    //        includeNumbers,
    //        includeSpecial,
    //        password.data(),  // Note: std::string::data() returns char*
    //        password.size()
    //    );
    //
    // 4. Check result
    //    if (result != 0) {
    //        throw RandomGenerationError("Failed to generate password");
    //    }
    //
    // 5. Resize to actual length (remove null terminator)
    //    password.resize(length);
    //
    // 6. Return the password
    //    return password;
    
    throw std::runtime_error("Not implemented: generatePassword");
}

// ============================================================================
// STEP 3: IMPLEMENT KEY DERIVATION METHODS
// ============================================================================

std::vector<uint8_t> CryptoEngine::deriveKey(
    const std::string& password,
    const std::vector<uint8_t>& salt)
{
    // TODO: Implement key derivation
    //
    // Steps:
    // 1. Validate inputs
    //    if (password.empty()) {
    //        throw std::invalid_argument("Password cannot be empty");
    //    }
    //    if (salt.size() != SALT_SIZE) {
    //        throw std::invalid_argument("Invalid salt size");
    //    }
    //
    // 2. Create output buffer
    //    std::vector<uint8_t> key(KEY_SIZE);
    //
    // 3. Call Rust function
    //    int result = rust_derive_key(
    //        password.c_str(),  // Convert string to C string
    //        salt.data(),       // Get pointer to salt bytes
    //        salt.size(),       // Salt length
    //        key.data(),        // Output buffer
    //        key.size()         // Output buffer size
    //    );
    //
    // 4. Check result
    //    if (result != 0) {
    //        throw KeyDerivationError("Failed to derive key from password");
    //    }
    //
    // 5. Return the key
    //    return key;
    //
    // NOTE: This operation is intentionally slow (Argon2)
    // It may take 100ms - 1s depending on parameters
    
    throw std::runtime_error("Not implemented: deriveKey");
}

std::vector<uint8_t> CryptoEngine::hashKeyForStorage(
    const std::vector<uint8_t>& key)
{
    // TODO: Implement key hashing for storage
    //
    // Steps:
    // 1. Validate key size
    //    if (key.size() != KEY_SIZE) {
    //        throw std::invalid_argument("Invalid key size");
    //    }
    //
    // 2. Create output buffer
    //    std::vector<uint8_t> hash(HASH_SIZE);
    //
    // 3. Call Rust function
    //    int result = rust_hash_key_for_storage(
    //        key.data(),
    //        key.size(),
    //        hash.data(),
    //        hash.size()
    //    );
    //
    // 4. Check result
    //    if (result != 0) {
    //        throw CryptoException("Failed to hash key");
    //    }
    //
    // 5. Return the hash
    //    return hash;
    
    throw std::runtime_error("Not implemented: hashKeyForStorage");
}

// ============================================================================
// STEP 4: IMPLEMENT ENCRYPTION METHODS
// ============================================================================

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> CryptoEngine::encrypt(
    const std::string& plaintext,
    const std::vector<uint8_t>& key)
{
    // TODO: Implement encryption
    //
    // Steps:
    // 1. Validate key size
    //    if (key.size() != KEY_SIZE) {
    //        throw std::invalid_argument("Invalid key size");
    //    }
    //
    // 2. Validate plaintext (not empty?)
    //    if (plaintext.empty()) {
    //        throw std::invalid_argument("Plaintext cannot be empty");
    //    }
    //
    // 3. Prepare buffers
    //    // Ciphertext = plaintext + 16 bytes for authentication tag
    //    std::vector<uint8_t> ciphertext(plaintext.size() + 16);
    //    std::vector<uint8_t> nonce(NONCE_SIZE);
    //    size_t ciphertext_len = ciphertext.size();
    //
    // 4. Convert plaintext to bytes
    //    const uint8_t* plaintext_bytes = 
    //        reinterpret_cast<const uint8_t*>(plaintext.data());
    //
    // 5. Call Rust function
    //    int result = rust_encrypt(
    //        plaintext_bytes,
    //        plaintext.size(),
    //        key.data(),
    //        key.size(),
    //        ciphertext.data(),
    //        &ciphertext_len,      // Note: pointer, will be updated
    //        nonce.data(),
    //        nonce.size()
    //    );
    //
    // 6. Check result
    //    if (result != 0) {
    //        throw EncryptionError("Encryption failed");
    //    }
    //
    // 7. Resize ciphertext to actual size
    //    ciphertext.resize(ciphertext_len);
    //
    // 8. Return both ciphertext and nonce
    //    return {ciphertext, nonce};
    //
    // CRITICAL: The caller MUST save the nonce!
    // Without the nonce, decryption is impossible!
    
    throw std::runtime_error("Not implemented: encrypt");
}

std::string CryptoEngine::decrypt(
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& key,
    const std::vector<uint8_t>& nonce)
{
    // TODO: Implement decryption
    //
    // Steps:
    // 1. Validate inputs
    //    if (key.size() != KEY_SIZE) {
    //        throw std::invalid_argument("Invalid key size");
    //    }
    //    if (nonce.size() != NONCE_SIZE) {
    //        throw std::invalid_argument("Invalid nonce size");
    //    }
    //    if (ciphertext.size() < 16) {
    //        throw std::invalid_argument("Ciphertext too short");
    //    }
    //
    // 2. Prepare output buffer
    //    std::vector<uint8_t> plaintext(ciphertext.size());
    //    size_t plaintext_len = plaintext.size();
    //
    // 3. Call Rust function
    //    int result = rust_decrypt(
    //        ciphertext.data(),
    //        ciphertext.size(),
    //        key.data(),
    //        key.size(),
    //        nonce.data(),
    //        nonce.size(),
    //        plaintext.data(),
    //        &plaintext_len         // Note: pointer, will be updated
    //    );
    //
    // 4. Check result
    //    if (result != 0) {
    //        // Decryption failed - could be wrong key, wrong nonce,
    //        // or tampered data (authentication failed)
    //        throw DecryptionError(
    //            "Decryption failed: wrong key, nonce, or corrupted data"
    //        );
    //    }
    //
    // 5. Resize plaintext to actual size
    //    plaintext.resize(plaintext_len);
    //
    // 6. Convert bytes to string
    //    std::string result_str(plaintext.begin(), plaintext.end());
    //
    // 7. Return the plaintext
    //    return result_str;
    //
    // SECURITY NOTE: If this throws, DO NOT return partial data!
    // Authentication failure means data was tampered with.
    
    throw std::runtime_error("Not implemented: decrypt");
}

// ============================================================================
// OPTIONAL: IMPLEMENT UTILITY METHODS
// ============================================================================

// TODO (Optional): Implement secure memory clearing
//
// void CryptoEngine::secureZero(std::vector<uint8_t>& data) {
//     #ifdef _WIN32
//         SecureZeroMemory(data.data(), data.size());
//     #else
//         explicit_bzero(data.data(), data.size());
//     #endif
//     data.clear();
// }
//
// void CryptoEngine::secureZero(std::string& str) {
//     #ifdef _WIN32
//         SecureZeroMemory(str.data(), str.size());
//     #else
//         explicit_bzero(str.data(), str.size());
//     #endif
//     str.clear();
// }

// ============================================================================
// TESTING NOTES
// ============================================================================
//
// To test this implementation:
//
// 1. Create a simple test in main.cpp:
//    CryptoEngine crypto;
//    
//    // Test salt generation
//    auto salt = crypto.generateSalt();
//    std::cout << "Salt size: " << salt.size() << std::endl;
//    
//    // Test key derivation
//    auto key = crypto.deriveKey("mypassword", salt);
//    std::cout << "Key size: " << key.size() << std::endl;
//    
//    // Test encryption
//    auto [ciphertext, nonce] = crypto.encrypt("secret", key);
//    std::cout << "Encrypted size: " << ciphertext.size() << std::endl;
//    
//    // Test decryption
//    auto plaintext = crypto.decrypt(ciphertext, key, nonce);
//    std::cout << "Decrypted: " << plaintext << std::endl;
//
// 2. Build and run
//
// 3. Verify:
//    - Sizes are correct
//    - Decrypted text matches original
//    - Different salts give different keys
//    - Different nonces give different ciphertexts
//    - Wrong key fails to decrypt
//    - Wrong nonce fails to decrypt
//
// 4. Check for memory leaks with Valgrind
//
// ============================================================================
