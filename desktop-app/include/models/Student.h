#ifndef STUDENT_H
#define STUDENT_H

#include "models/User.h"

/**
 * @file Student.h
 * @brief Student model derived from User.
 * 
 * Demonstrates OOP Concepts:
 * - Inheritance: Inherits identity and core credentials from User.
 * - Polymorphism: Overrides getRole() and getSpecificId().
 * - Encapsulation: Protects student academic stats and student ID.
 */
class Student : public User {
private:
    QString m_studentId;
    int m_totalExamsTaken;
    double m_averageScore;

public:
    Student();
    Student(int id, const QString& username, const QString& fullName,
            const QString& email, const QString& studentId,
            int totalExamsTaken = 0, double avgScore = 0.0,
            const QDateTime& createdAt = QDateTime::currentDateTime());
    ~Student() override = default;

    // Polymorphic overrides
    QString getRole() const override { return "Student"; }
    QString getSpecificId() const override { return m_studentId; }

    // Student specific getters and setters
    QString getStudentId() const { return m_studentId; }
    void setStudentId(const QString& studentId) { m_studentId = studentId.trimmed(); }

    int getTotalExamsTaken() const { return m_totalExamsTaken; }
    void setTotalExamsTaken(int count) { m_totalExamsTaken = count >= 0 ? count : 0; }

    double getAverageScore() const { return m_averageScore; }
    void setAverageScore(double avg) { m_averageScore = avg >= 0.0 ? avg : 0.0; }

    bool isValid() const override {
        return User::isValid() && !m_studentId.trimmed().isEmpty();
    }
};

using StudentPtr = std::shared_ptr<Student>;

#endif // STUDENT_H
