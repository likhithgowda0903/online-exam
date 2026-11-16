#include "models/User.h"

User::User() 
    : m_id(0), m_username(""), m_fullName(""), m_email(""), m_role(""), m_createdAt(QDateTime::currentDateTime()) {
}

User::User(int id, const QString& username, const QString& fullName, 
           const QString& email, const QString& role, const QDateTime& createdAt)
    : m_id(id), m_username(username), m_fullName(fullName), 
      m_email(email), m_role(role), m_createdAt(createdAt) {
}
