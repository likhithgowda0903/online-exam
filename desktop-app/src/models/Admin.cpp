#include "models/Admin.h"

Admin::Admin()
    : User(0, "", "", "", "Admin"), m_adminCode(""), m_department("Academic Affairs") {
}

Admin::Admin(int id, const QString& username, const QString& fullName,
             const QString& email, const QString& adminCode,
             const QString& department, const QDateTime& createdAt)
    : User(id, username, fullName, email, "Admin", createdAt),
      m_adminCode(adminCode), m_department(department) {
}
