#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <memory>

/**
 * @file User.h
 * @brief Abstract Base Class representing a system user.
 * 
 * Demonstrates OOP Concepts:
 * - Abstraction: Pure virtual method getRole() and common user interface.
 * - Encapsulation: Private member variables accessible only through getters/setters.
 */
class User {
protected:
    int m_id;
    QString m_username;
    QString m_fullName;
    QString m_email;
    QString m_role;
    QDateTime m_createdAt;

public:
    // Constructors & Destructor
    User();
    User(int id, const QString& username, const QString& fullName, 
         const QString& email, const QString& role, const QDateTime& createdAt = QDateTime::currentDateTime());
    virtual ~User() = default;

    // Pure virtual function enforcing polymorphic implementation in derived classes
    virtual QString getRole() const = 0;

    // Virtual methods allowing derived classes to customize behavior
    virtual QString getSpecificId() const { return ""; }
    virtual QString getDisplayName() const { return m_fullName.isEmpty() ? m_username : m_fullName; }

    // Getters (Encapsulation)
    int getId() const { return m_id; }
    QString getUsername() const { return m_username; }
    QString getFullName() const { return m_fullName; }
    QString getEmail() const { return m_email; }
    QDateTime getCreatedAt() const { return m_createdAt; }

    // Setters (Encapsulation with validation)
    void setId(int id) { m_id = id; }
    void setUsername(const QString& username) { m_username = username.trimmed(); }
    void setFullName(const QString& fullName) { m_fullName = fullName.trimmed(); }
    void setEmail(const QString& email) { m_email = email.trimmed(); }
    void setCreatedAt(const QDateTime& dt) { m_createdAt = dt; }

    // Helper validation
    virtual bool isValid() const {
        return !m_username.trimmed().isEmpty() && !m_email.trimmed().isEmpty();
    }
};

using UserPtr = std::shared_ptr<User>;

#endif // USER_H
