#include "database/PasswordHasher.h"

QString PasswordHasher::generateSalt(int length) {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*");
    QString randomString;
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        randomString.append(possibleCharacters.at(index));
    }
    return randomString;
}

QString PasswordHasher::hashPassword(const QString& plainText, const QString& salt) {
    QByteArray combined = (salt + plainText + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(combined, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool PasswordHasher::verifyPassword(const QString& plainText, const QString& storedHash, const QString& salt) {
    QString computedHash = hashPassword(plainText, salt);
    return computedHash == storedHash;
}
