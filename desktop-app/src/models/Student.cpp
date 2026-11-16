#include "models/Student.h"

Student::Student()
    : User(0, "", "", "", "Student"), m_studentId(""), m_totalExamsTaken(0), m_averageScore(0.0) {
}

Student::Student(int id, const QString& username, const QString& fullName,
                 const QString& email, const QString& studentId,
                 int totalExamsTaken, double avgScore,
                 const QDateTime& createdAt)
    : User(id, username, fullName, email, "Student", createdAt),
      m_studentId(studentId), m_totalExamsTaken(totalExamsTaken), m_averageScore(avgScore) {
}
