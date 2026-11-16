#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Pure C++17 Cryptographic SHA-256 and Salting Engine
 * FIPS 180-2 compliant SHA-256 implementation with zero external dependencies.
 */
class Sha256 {
public:
    Sha256();
    void update(const uint8_t* data, size_t length);
    void update(const std::string& data);
    std::string finalize();

    // Static helper methods matching Online Examination System authentication standards
    static std::string hash(const std::string& input);
    static std::string hashPassword(const std::string& plainText, const std::string& salt);
    static std::string generateSalt(int length = 16);
    static bool verifyPassword(const std::string& plainText, const std::string& storedHash, const std::string& salt);

private:
    void transform(const uint8_t* chunk);
    void pad();
    void revert();

    uint32_t m_state[8];
    uint64_t m_bitCount;
    uint8_t m_buffer[64];
};

#endif // SHA256_H

