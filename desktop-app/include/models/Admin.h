#ifndef ADMIN_H
#define ADMIN_H

#include "models/User.h"

/**
 * @file Admin.h
 * @brief Admin model derived from User.
 * 
 * Demonstrates OOP Concepts:
 * - Inheritance: Extends base User class with administrative capabilities.
 * - Polymorphism: Overrides getRole() returning "Admin" and getSpecificId().
 * - Encapsulation: Stores departmental and administrative privileges privately.
 */
class Admin : public User {
private:
    QString m_adminCode;
    QString m_department;

public:
    Admin();
    Admin(int id, const QString& username, const QString& fullName,
          const QString& email, const QString& adminCode,
          const QString& department = "Academic Affairs",
          const QDateTime& createdAt = QDateTime::currentDateTime());
    ~Admin() override = default;

    // Polymorphic overrides
    QString getRole() const override { return "Admin"; }
    QString getSpecificId() const override { return m_adminCode; }

    // Admin specific getters & setters
    QString getAdminCode() const { return m_adminCode; }
    void setAdminCode(const QString& code) { m_adminCode = code.trimmed(); }

    QString getDepartment() const { return m_department; }
    void setDepartment(const QString& dept) { m_department = dept.trimmed(); }
};

using AdminPtr = std::shared_ptr<Admin>;

#endif // ADMIN_H
