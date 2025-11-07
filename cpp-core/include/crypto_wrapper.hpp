// ============================================================================
// CRYPTIFY - Crypto Wrapper Header
// ============================================================================
// This file provides a safe, idiomatic C++ interface to the Rust cryptographic
// functions. It handles type conversions and error handling.
//
// Key concepts:
// - Wraps raw C FFI functions from Rust
// - Provides type-safe C++ interface
// - Uses RAII for resource management
// - Throws exceptions on errors instead of returning error codes
// ============================================================================

#ifndef CRYPTO_WRAPPER_HPP
#define CRYPTO_WRAPPER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <utility>

// ============================================================================
// STEP 1: DECLARE THE RUST FFI FUNCTIONS
// ============================================================================

// TODO: Declare all Rust functions here in an extern "C" block
// These must match exactly what Rust exports (check your lib.rs)
//
// Example structure:
// extern "C" {
//     int rust_generate_salt(uint8_t* salt_out, size_t salt_len);
//     int rust_generate_nonce(uint8_t* nonce_out, size_t nonce_len);
//     // ... add all other Rust functions
// }
//
// Remember:
// - Use C types (uint8_t*, size_t, int, char*, bool)
// - Match parameter order exactly
// - Match return types exactly

extern "C" {
    // TODO: Declare rust_generate_salt
    // Parameters: salt_out (buffer), salt_len (buffer size)
    // Returns: 0 on success, error code on failure
    
    // TODO: Declare rust_generate_nonce
    // Parameters: nonce_out (buffer), nonce_len (buffer size)
    // Returns: 0 on success, error code on failure
    
    // TODO: Declare rust_generate_password
    // Parameters: length, flags for char types, password_out (buffer), buffer_len
    // Returns: 0 on success, error code on failure
    
    // TODO: Declare rust_derive_key
    // Parameters: password (C string), salt (bytes), salt_len, key_out (buffer), key_len
    // Returns: 0 on success, error code on failure
    
    // TODO: Declare rust_hash_key_for_storage
    // Parameters: key (bytes), key_len, hash_out (buffer), hash_len
    // Returns: 0 on success, error code on failure
    
    // TODO: Declare rust_encrypt
    // Parameters: plaintext, plaintext_len, key, key_len, 
    //             ciphertext_out, ciphertext_len (in/out), nonce_out, nonce_len
    // Returns: 0 on success, error code on failure
    
    // TODO: Declare rust_decrypt
    // Parameters: ciphertext, ciphertext_len, key, key_len, nonce, nonce_len,
    //             plaintext_out, plaintext_len (in/out)
    // Returns: 0 on success, error code on failure
}

// ============================================================================
// STEP 2: DEFINE CUSTOM EXCEPTION CLASSES
// ============================================================================

// TODO: Create exception hierarchy for crypto errors
//
// Base class: CryptoException
// Derived classes:
// - KeyDerivationError
// - EncryptionError
// - DecryptionError
// - RandomGenerationError
//
// Each should:
// - Inherit from std::runtime_error
// - Have a constructor that takes a message string
// - Provide meaningful error messages

// Base exception for all crypto operations
class CryptoException : public std::runtime_error {
public:
    // TODO: Implement constructor
    // Should call std::runtime_error constructor with message
    explicit CryptoException(const std::string& message);
};

// TODO: Define KeyDerivationError
// Used when key derivation fails (e.g., invalid parameters)

// TODO: Define EncryptionError
// Used when encryption fails

// TODO: Define DecryptionError
// Used when decryption fails (wrong key, corrupted data, etc.)

// TODO: Define RandomGenerationError
// Used when random number generation fails

// ============================================================================
// STEP 3: DEFINE THE CRYPTOENGINE CLASS
// ============================================================================

class CryptoEngine {
public:
    // ========================================================================
    // CONSTANTS
    // ========================================================================
    
    // TODO: Define cryptographic constants
    // These should match what you defined in Rust
    static constexpr size_t KEY_SIZE = 32;      // 256 bits for AES-256
    static constexpr size_t SALT_SIZE = 16;     // 128 bits
    static constexpr size_t NONCE_SIZE = 12;    // 96 bits for GCM
    static constexpr size_t HASH_SIZE = 32;     // 256 bits for SHA-256
    
    // ========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    // ========================================================================
    
    // TODO: Implement default constructor
    // The CryptoEngine is stateless, so no initialization needed
    CryptoEngine() = default;
    
    // TODO: Implement destructor
    // Default is fine since we have no resources to clean up
    ~CryptoEngine() = default;
    
    // ========================================================================
    // DELETE COPY OPERATIONS
    // ========================================================================
    
    // TODO: Delete copy constructor and copy assignment
    // Crypto operations shouldn't be copied
    // Use = delete syntax
    CryptoEngine(const CryptoEngine&) = delete;
    CryptoEngine& operator=(const CryptoEngine&) = delete;
    
    // TODO: You can allow move operations if needed (optional)
    // CryptoEngine(CryptoEngine&&) = default;
    // CryptoEngine& operator=(CryptoEngine&&) = default;
    
    // ========================================================================
    // RANDOM GENERATION METHODS
    // ========================================================================
    
    // TODO: Implement generateSalt()
    // What it does: Generate a random salt for key derivation
    // 
    // Steps:
    // 1. Create a vector of size SALT_SIZE
    // 2. Call rust_generate_salt with vector data pointer
    // 3. Check return code
    // 4. If error, throw RandomGenerationError
    // 5. Return the vector
    //
    // Returns: vector<uint8_t> of length SALT_SIZE
    // Throws: RandomGenerationError on failure
    std::vector<uint8_t> generateSalt();
    
    // TODO: Implement generateNonce()
    // What it does: Generate a random nonce for encryption
    //
    // Steps: Same as generateSalt but with NONCE_SIZE
    // 1. Create a vector of size NONCE_SIZE
    // 2. Call rust_generate_nonce
    // 3. Check return code and throw on error
    // 4. Return the vector
    //
    // Returns: vector<uint8_t> of length NONCE_SIZE
    // Throws: RandomGenerationError on failure
    std::vector<uint8_t> generateNonce();
    
    // TODO: Implement generatePassword()
    // What it does: Generate a strong random password
    //
    // Parameters:
    // - length: Desired password length (suggest 12-32)
    // - includeUppercase: Include uppercase letters (A-Z)
    // - includeNumbers: Include numbers (0-9)
    // - includeSpecial: Include special characters (!@#$%^&* etc.)
    //
    // Steps:
    // 1. Validate length (minimum 8, maximum 128?)
    // 2. Create a buffer (length + 1 for null terminator)
    // 3. Call rust_generate_password with parameters
    // 4. Check return code and throw on error
    // 5. Convert C string to std::string
    // 6. Return the string
    //
    // Returns: string containing the generated password
    // Throws: RandomGenerationError on failure
    std::string generatePassword(
        size_t length,
        bool includeUppercase = true,
        bool includeNumbers = true,
        bool includeSpecial = true
    );
    
    // ========================================================================
    // KEY DERIVATION METHODS
    // ========================================================================
    
    // TODO: Implement deriveKey()
    // What it does: Derive an encryption key from password using Argon2
    //
    // Parameters:
    // - password: The master password (string)
    // - salt: The salt to use (must be unique per user)
    //
    // Steps:
    // 1. Validate inputs (password not empty, salt is SALT_SIZE)
    // 2. Create output vector of size KEY_SIZE
    // 3. Call rust_derive_key with:
    //    - password.c_str() (convert to C string)
    //    - salt.data() (get pointer to salt bytes)
    //    - salt.size()
    //    - key_out.data()
    //    - KEY_SIZE
    // 4. Check return code and throw on error
    // 5. Return the derived key
    //
    // IMPORTANT: This is a slow operation (intentionally)
    // Argon2 makes brute-force attacks expensive
    //
    // Returns: vector<uint8_t> of length KEY_SIZE (32 bytes)
    // Throws: KeyDerivationError on failure
    std::vector<uint8_t> deriveKey(
        const std::string& password,
        const std::vector<uint8_t>& salt
    );
    
    // TODO: Implement hashKeyForStorage()
    // What it does: Hash the derived key for database storage
    //
    // Parameters:
    // - key: The derived key (from deriveKey)
    //
    // Steps:
    // 1. Validate key size (should be KEY_SIZE)
    // 2. Create output vector of size HASH_SIZE
    // 3. Call rust_hash_key_for_storage
    // 4. Check return code and throw on error
    // 5. Return the hash
    //
    // This hash is stored in the database to verify login attempts
    // It cannot be reversed to get the key back
    //
    // Returns: vector<uint8_t> of length HASH_SIZE (32 bytes)
    // Throws: CryptoException on failure
    std::vector<uint8_t> hashKeyForStorage(
        const std::vector<uint8_t>& key
    );
    
    // ========================================================================
    // ENCRYPTION METHODS
    // ========================================================================
    
    // TODO: Implement encrypt()
    // What it does: Encrypt plaintext using AES-256-GCM
    //
    // Parameters:
    // - plaintext: The data to encrypt (as string)
    // - key: The encryption key (from deriveKey, must be KEY_SIZE)
    //
    // Steps:
    // 1. Validate key size (must be KEY_SIZE = 32)
    // 2. Create ciphertext buffer (plaintext.size() + 16 for auth tag)
    // 3. Create nonce buffer (NONCE_SIZE = 12)
    // 4. Initialize ciphertext_len variable with buffer size
    // 5. Call rust_encrypt with:
    //    - plaintext as uint8_t* (reinterpret_cast)
    //    - plaintext.size()
    //    - key.data()
    //    - key.size()
    //    - ciphertext_out.data()
    //    - &ciphertext_len (pointer, will be updated)
    //    - nonce_out.data()
    //    - nonce_out.size()
    // 6. Check return code and throw on error
    // 7. Resize ciphertext to actual size (ciphertext_len)
    // 8. Return pair of {ciphertext, nonce}
    //
    // CRITICAL: The nonce MUST be saved along with ciphertext!
    // You need the same nonce to decrypt later!
    //
    // Returns: pair<vector<uint8_t>, vector<uint8_t>>
    //          First = ciphertext (includes auth tag)
    //          Second = nonce (must be saved!)
    // Throws: EncryptionError on failure
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> encrypt(
        const std::string& plaintext,
        const std::vector<uint8_t>& key
    );
    
    // TODO: Implement decrypt()
    // What it does: Decrypt ciphertext using AES-256-GCM
    //
    // Parameters:
    // - ciphertext: The encrypted data (includes auth tag)
    // - key: Same encryption key used to encrypt (KEY_SIZE)
    // - nonce: Same nonce used during encryption (NONCE_SIZE)
    //
    // Steps:
    // 1. Validate key size (must be KEY_SIZE)
    // 2. Validate nonce size (must be NONCE_SIZE)
    // 3. Validate ciphertext size (at least 16 for tag)
    // 4. Create plaintext buffer (ciphertext.size())
    // 5. Initialize plaintext_len variable with buffer size
    // 6. Call rust_decrypt with:
    //    - ciphertext.data()
    //    - ciphertext.size()
    //    - key.data()
    //    - key.size()
    //    - nonce.data()
    //    - nonce.size()
    //    - plaintext_out.data()
    //    - &plaintext_len (pointer, will be updated)
    // 7. Check return code:
    //    - If error, throw DecryptionError
    //    - Error can mean: wrong key, wrong nonce, or tampered data
    // 8. Resize plaintext to actual size
    // 9. Convert bytes to string
    // 10. Return the plaintext string
    //
    // SECURITY: If decryption fails, the data was tampered with or
    // the wrong key/nonce was used. DO NOT return partial data!
    //
    // Returns: string containing the decrypted plaintext
    // Throws: DecryptionError on failure (wrong key, corrupted data, etc.)
    std::string decrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& key,
        const std::vector<uint8_t>& nonce
    );
    
    // ========================================================================
    // OPTIONAL: UTILITY METHODS
    // ========================================================================
    
    // TODO (Optional): Implement secure memory clearing
    // C++ doesn't guarantee that strings/vectors are zeroed when destroyed
    // For production, consider implementing:
    //
    // static void secureZero(std::vector<uint8_t>& data);
    // static void secureZero(std::string& str);
    //
    // These would use:
    // - explicit_bzero() on Linux
    // - SecureZeroMemory() on Windows
    // - volatile pointer trick as fallback
};

#endif // CRYPTO_WRAPPER_HPP

// ============================================================================
// IMPLEMENTATION NOTES
// ============================================================================
//
// After defining this header, implement all methods in crypto_wrapper.cpp
//
// Testing strategy:
// 1. Test each method individually
// 2. Test error cases (null pointers, wrong sizes, etc.)
// 3. Test round-trip (encrypt then decrypt = original)
// 4. Test with different keys (should fail to decrypt)
// 5. Test with different nonces (should fail to decrypt)
// 6. Verify exceptions are thrown correctly
//
// Common issues:
// - Buffer sizes: Make sure they match Rust expectations
// - Pointer lifetimes: Ensure buffers live until Rust call completes
// - Type conversions: Use reinterpret_cast carefully
// - Error checking: Always check Rust return codes
//
// Security considerations:
// - Clear sensitive data from memory after use
// - Don't log passwords or keys
// - Validate all inputs before calling Rust
// - Handle exceptions properly (don't leak information)
//
// ============================================================================
