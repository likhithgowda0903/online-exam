#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <QString>
#include <QCryptographicHash>
#include <QRandomGenerator>

/**
 * @file PasswordHasher.h
 * @brief Cryptographic password hashing and verification utility.
 * 
 * Ensures that plain-text passwords are never stored in the database.
 * Uses SHA-256 with cryptographic salting.
 */
class PasswordHasher {
public:
    // Generate a random cryptographic salt
    static QString generateSalt(int length = 16);

    // Hash plain-text password combined with salt using SHA-256
    static QString hashPassword(const QString& plainText, const QString& salt);

    // Verify if provided plain password matches stored hash
    static bool verifyPassword(const QString& plainText, const QString& storedHash, const QString& salt);
};

#endif // PASSWORD_HASHER_H
