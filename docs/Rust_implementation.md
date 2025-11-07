# Cryptify - Your Next Steps

## Current Status ✅
You've successfully:
- ✅ Set up your development environment
- ✅ Created the project structure
- ✅ Built your first Rust library
- ✅ Created a C++ program that calls Rust
- ✅ Verified FFI (Foreign Function Interface) works!

---

## Phase 2: Build the Rust Cryptography Layer

### Your Goal
Create a Rust library that handles all cryptographic operations safely. This will be the foundation of your password manager's security.

---

### Step 1: Learn About the Crypto Primitives You'll Need

**Before writing code, understand these concepts:**

#### 1.1 Key Derivation (Argon2)
**What it does:** Converts your master password into an encryption key

**Why you need it:**
- Users type passwords, but encryption needs fixed-length keys
- Makes brute-force attacks slow and expensive
- Protects against GPU/ASIC attacks

**What to learn:**
- What is a "salt" and why is it random?
- What are "iterations" or "time cost"?
- What is "memory cost"?
- Why Argon2 is better than PBKDF2 or bcrypt

**Resources to read:**
- Argon2 documentation
- "How to hash passwords" articles
- Understand the difference: hashing vs encryption

#### 1.2 Encryption (AES-256-GCM)
**What it does:** Encrypts your passwords so they can't be read without the key

**Why you need it:**
- Symmetric encryption (same key encrypts and decrypts)
- GCM mode provides authentication (detects tampering)
- Industry standard, battle-tested

**What to learn:**
- What is a "nonce" (number used once)?
- Why must nonces NEVER be reused?
- What is an "authentication tag"?
- Why AES-GCM is better than AES-CBC

**Resources to read:**
- AES-GCM explained
- Authenticated encryption concepts
- Nonce importance and generation

#### 1.3 Random Number Generation
**What it does:** Creates unpredictable values (salts, nonces, passwords)

**Why you need it:**
- Cryptographic operations need true randomness
- Weak randomness = weak security
- Used for salts, nonces, and password generation

**What to learn:**
- Difference between random and cryptographically secure random
- How your OS provides entropy
- Why you should never implement your own RNG

#### 1.4 Hashing (for Master Password)
**What it does:** One-way function to verify passwords without storing them

**Why you need it:**
- You NEVER store the master password
- You store a hash to verify login attempts
- Can't reverse a hash to get the original password

**What to learn:**
- Difference between hashing and encryption
- Why hash functions are one-way
- Salt + hash = protection against rainbow tables

---

### Step 2: Design Your Rust FFI Interface

**Think about what functions C++ needs to call:**

#### Functions you'll need to implement:

1. **`rust_derive_key`**
   - Input: password (string), salt (bytes)
   - Output: derived key (32 bytes)
   - Purpose: Turn password into encryption key

2. **`rust_encrypt_password`**
   - Input: plaintext (string), key (bytes)
   - Output: ciphertext (bytes), nonce (bytes)
   - Purpose: Encrypt a password for storage

3. **`rust_decrypt_password`**
   - Input: ciphertext (bytes), key (bytes), nonce (bytes)
   - Output: plaintext (string)
   - Purpose: Decrypt a stored password

4. **`rust_generate_salt`**
   - Input: desired length
   - Output: random bytes
   - Purpose: Create unique salt for each user

5. **`rust_generate_nonce`**
   - Input: (none - always 12 bytes for GCM)
   - Output: random 12 bytes
   - Purpose: Create unique nonce for each encryption

6. **`rust_generate_password`**
   - Input: length, options (include special chars?, numbers?, etc.)
   - Output: random password string
   - Purpose: Generate strong passwords for users

7. **`rust_hash_for_storage`**
   - Input: derived key
   - Output: hash for database storage
   - Purpose: Verify master password on login

---

### Step 3: Understand Memory Safety at FFI Boundary

**Critical concepts to learn:**

#### 3.1 Who Owns the Memory?
- When C++ passes a pointer to Rust: C++ owns it
- When Rust returns data to C++: Who frees it?
- Options:
  - C++ allocates, Rust fills in
  - Rust allocates, C++ must free
  - Use a buffer pattern (safer)

**Best practice for this project:**
- C++ allocates buffers
- C++ passes buffer + size to Rust
- Rust fills the buffer
- C++ owns and manages memory

#### 3.2 String Handling
- C++ strings are NOT the same as Rust strings
- Use `const char*` for FFI (C-style strings)
- Always null-terminated
- Always check for null pointers in Rust

#### 3.3 Binary Data (Bytes)
- Use `*const u8` and `*mut u8` for byte arrays
- Always pass length with pointer
- Never assume null-termination for binary data

#### 3.4 Error Handling
- Rust panics = crash in C++
- Never panic across FFI boundary
- Use return codes:
  - 0 = success
  - -1 = null pointer
  - -2 = invalid length
  - -3 = encryption failed
  - etc.

---

### Step 4: Learn the Rust Crypto Libraries

**Libraries you added to Cargo.toml:**

#### 4.1 `aes-gcm` crate
**What to learn:**
- How to create an `Aes256Gcm` cipher
- How to create a `Key` and `Nonce`
- How to call `encrypt()` and `decrypt()`
- How to handle the authentication tag
- Error handling (what errors can occur?)

**Read:**
- aes-gcm crate documentation
- Examples in the docs
- GitHub issues for common problems

#### 4.2 `argon2` crate
**What to learn:**
- How to create an Argon2 hasher
- What parameters to use (time, memory, parallelism)
- How to derive a key from password + salt
- Recommended parameters for password hashing

**Read:**
- argon2 crate documentation
- OWASP password storage guidelines
- Argon2 RFC

#### 4.3 `rand` crate
**What to learn:**
- How to use `rand::thread_rng()`
- How to generate random bytes
- How to generate random characters for passwords
- Character sets (uppercase, lowercase, numbers, special)

#### 4.4 `zeroize` crate
**What to learn:**
- Why you need to zero sensitive memory
- How to use `Zeroizing` wrapper
- When to zeroize (keys, passwords, plaintext)
- Preventing sensitive data from staying in memory

**This is CRITICAL for security!**

#### 4.5 `base64` crate
**What to learn:**
- How to encode binary data to base64
- How to decode base64 to bytes
- When you might need this (storing binary in text fields)

---

### Step 5: Plan Your Implementation Order

**Do things in this order (test each before moving on):**

1. **Generate random salt** (simplest)
   - Just wrap `rand` functions
   - Test: generate multiple salts, verify they're different

2. **Generate random password** (still simple)
   - Choose character set
   - Generate random indices
   - Build string
   - Test: check length, character variety

3. **Key derivation** (getting harder)
   - Set up Argon2 parameters
   - Convert types properly
   - Test: same password + salt = same key always

4. **Hash for storage** (medium)
   - Hash the derived key
   - Store this hash to verify login
   - Test: verify hash matches on same input

5. **Encryption** (complex)
   - Set up AES-GCM cipher
   - Generate nonce
   - Encrypt data
   - Return both ciphertext and nonce
   - Test: encrypt same data multiple times = different ciphertext

6. **Decryption** (complex)
   - Use saved nonce
   - Decrypt ciphertext
   - Handle authentication failure
   - Test: decrypt what you encrypted

7. **FFI wrappers** (tricky)
   - Wrap each function for C
   - Handle pointer conversions
   - Add error handling
   - Test: call from C++

---

### Step 6: Testing Strategy

**For each function you implement:**

#### Unit Tests in Rust
```
Create tests in your lib.rs or tests/ directory
Test normal cases
Test edge cases (empty strings, max length, etc.)
Test error cases (null pointers, invalid data)
```

#### Integration Tests with C++
```
After FFI wrappers are done
Call from your main.cpp test program
Verify results match expectations
Test error conditions
```

#### Security Tests
```
Verify nonces are unique every time
Verify salts are unique
Verify same password + salt = same key
Verify different nonces = different ciphertext
Verify wrong key = decryption failure
```

---

### Step 7: Common Pitfalls to Avoid

**Things that WILL go wrong (and how to avoid them):**

#### 7.1 Nonce Reuse
- ❌ **NEVER** reuse a nonce with the same key
- ✅ Generate a fresh random nonce for EVERY encryption
- 🔥 Reusing nonces completely breaks GCM security

#### 7.2 Not Clearing Sensitive Memory
- ❌ Leaving passwords/keys in memory after use
- ✅ Use `zeroize` to clear buffers
- 🔥 Sensitive data can be read from memory dumps

#### 7.3 Weak Parameters
- ❌ Using too few iterations or too little memory
- ✅ Use recommended parameters (see OWASP)
- 🔥 Makes brute-force attacks feasible

#### 7.4 Panicking at FFI Boundary
- ❌ Using `unwrap()` or `expect()` in FFI functions
- ✅ Use proper error handling with Result
- 🔥 Panics across FFI = undefined behavior

#### 7.5 Timing Attacks
- ❌ Using `==` to compare passwords/hashes
- ✅ Use constant-time comparison functions
- 🔥 Attackers can learn information from timing

#### 7.6 Buffer Overflows at FFI
- ❌ Writing more data than buffer size
- ✅ Always check buffer lengths
- 🔥 Memory corruption and crashes

---

### Step 8: What Success Looks Like

**You've succeeded in Phase 2 when:**

- [ ] You can derive a key from password + salt
- [ ] You can encrypt a string and get ciphertext + nonce
- [ ] You can decrypt the ciphertext back to original string
- [ ] You can generate random salts (and they're always different)
- [ ] You can generate random passwords
- [ ] All functions have proper error handling
- [ ] You have FFI wrappers that C++ can call
- [ ] You have tests for each function
- [ ] You understand WHY each piece works
- [ ] Your C++ test program can call and verify each Rust function

---

### Step 9: Learning Resources

**Cryptography Concepts:**
- "Serious Cryptography" by Jean-Philippe Aumasson (book)
- "Crypto 101" (free online book)
- OWASP Password Storage Cheat Sheet
- Understanding AES-GCM (Google for good articles)
- Argon2 paper and RFC

**Rust FFI:**
- The Rustonomicon (FFI chapter)
- "The Rust Programming Language" book
- RustCrypto documentation (GitHub org)

**Practical Guides:**
- Search: "rust ffi tutorial"
- Search: "aes-gcm rust example"
- Search: "argon2 rust password hashing"
- Look at examples in the crate documentation

---

### Step 10: Questions to Answer As You Learn

**Test your understanding:**

1. Why do we need both a salt AND a nonce?
   - *Hint: They serve different purposes*

2. Can you decrypt data if you lose the nonce?
   - *Hint: No! The nonce is required for decryption*

3. Why can't we reverse a hash?
   - *Hint: Information is lost in hashing*

4. What happens if two passwords have the same salt?
   - *Hint: Bad if same password, ok if different passwords*

5. What happens if we encrypt two things with same key and nonce?
   - *Hint: Catastrophic security failure!*

6. Where is the master password stored?
   - *Hint: NOWHERE! Only its hash is stored*

7. Can we change a user's password without decrypting all their passwords?
   - *Hint: No - you need to decrypt and re-encrypt with new key*

---

## Time Estimate for Phase 2

**Realistic timeline:**
- Week 1: Learn concepts, read documentation
- Week 2: Implement basic functions (salt, password generation)
- Week 3: Implement crypto functions (derive, encrypt, decrypt)
- Week 4: Create FFI wrappers and test from C++
- Week 5: Testing, debugging, security review

**Don't rush!** Understanding is more important than speed.

---

## What NOT to Do Yet

**Resist the temptation to:**
- ❌ Start building the database (wait for Phase 3)
- ❌ Create the password manager logic (wait for Phase 4)
- ❌ Build the CLI (wait for Phase 5)
- ❌ Think about Docker (wait for Phase 6)

**Focus ONLY on:**
- ✅ Understanding cryptography concepts
- ✅ Implementing crypto functions in Rust
- ✅ Making them callable from C++
- ✅ Testing thoroughly

---

## Daily Learning Plan

**Suggested routine:**

**Day 1-3: Theory**
- Read about AES-GCM
- Read about Argon2
- Understand salts, nonces, keys
- Write notes in your own words

**Day 4-7: Simple Functions**
- Implement salt generation
- Implement password generation
- Test from Rust
- Test from C++

**Day 8-14: Key Derivation**
- Study Argon2 parameters
- Implement key derivation
- Implement hashing for storage
- Test thoroughly

**Day 15-21: Encryption/Decryption**
- Study AES-GCM
- Implement encryption
- Implement decryption
- Handle nonce generation
- Test edge cases

**Day 22-28: FFI & Integration**
- Create C FFI wrappers
- Test from C++ program
- Fix issues
- Security review

**Day 29-35: Testing & Documentation**
- Write comprehensive tests
- Document what you learned
- Review security checklist
- Prepare for Phase 3

---

## Your Mission

**Phase 2 Goal:**
Build a rock-solid, secure cryptography library in Rust that C++ can call safely.

**Remember:**
- Take your time
- Understand each concept before coding
- Test everything
- Security is hard - be paranoid
- It's okay to make mistakes (that's how you learn!)
- Use the resources
- Ask questions (search, read docs, forums)

---

## Next Document

When you finish Phase 2, I'll create "PHASE_3.md" with the database implementation guide!

**For now: Focus on crypto, ignore everything else.**

Good luck! 🚀🔐

---

## Progress Tracker

Use this to track your learning:

### Week 1: Understanding
- [ ] Read about AES-GCM encryption
- [ ] Read about Argon2 key derivation
- [ ] Understand salt vs nonce
- [ ] Understand hashing vs encryption
- [ ] Read FFI safety guidelines

### Week 2-3: Implementation
- [ ] Generate random salt
- [ ] Generate random password
- [ ] Derive key from password + salt
- [ ] Hash key for storage
- [ ] Encrypt data with AES-GCM
- [ ] Decrypt data with AES-GCM

### Week 4: FFI & Testing
- [ ] Create C-compatible wrapper functions
- [ ] Test each function from C++
- [ ] Write Rust unit tests
- [ ] Integration tests with C++
- [ ] Security verification

### Week 5: Review
- [ ] All functions work correctly
- [ ] No panics in FFI code
- [ ] Proper error handling
- [ ] Memory is zeroized
- [ ] Ready for Phase 3

