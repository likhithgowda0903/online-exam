#ifndef RESULT_H
#define RESULT_H

#include <QString>
#include <QDateTime>

/**
 * @file Result.h
 * @brief Result model capturing an exam attempt scorecard.
 * 
 * Demonstrates Encapsulation:
 * - Stores all examination performance metrics.
 * - Provides formatted reporting helpers for UI scorecard and history tables.
 */
class Result {
private:
    int m_id;
    int m_studentId;
    QString m_studentUsername;
    QString m_studentFullName;
    int m_quizId;
    QString m_quizTitle;
    int m_totalQuestions;
    int m_correctAnswers;
    int m_incorrectAnswers;
    int m_unansweredQuestions;
    int m_totalMarks;
    int m_marksObtained;
    double m_percentage;
    bool m_isPassed;
    QDateTime m_attemptDate;
    int m_timeTakenSeconds;
    int m_proctorWarnings;
    double m_integrityScore;

public:
    Result();
    Result(int id, int studentId, int quizId,
           int totalQuestions, int correctAnswers, int incorrectAnswers, int unansweredQuestions,
           int totalMarks, int marksObtained, double percentage, bool isPassed,
           const QDateTime& attemptDate = QDateTime::currentDateTime(), int timeTakenSeconds = 0,
           const QString& studentUsername = "", const QString& studentFullName = "",
           const QString& quizTitle = "", int proctorWarnings = 0, double integrityScore = 100.0);

    // Getters
    int getId() const { return m_id; }
    int getStudentId() const { return m_studentId; }
    QString getStudentUsername() const { return m_studentUsername; }
    QString getStudentFullName() const { return m_studentFullName; }
    int getQuizId() const { return m_quizId; }
    QString getQuizTitle() const { return m_quizTitle; }
    int getTotalQuestions() const { return m_totalQuestions; }
    int getCorrectAnswers() const { return m_correctAnswers; }
    int getIncorrectAnswers() const { return m_incorrectAnswers; }
    int getUnansweredQuestions() const { return m_unansweredQuestions; }
    int getTotalMarks() const { return m_totalMarks; }
    int getMarksObtained() const { return m_marksObtained; }
    double getPercentage() const { return m_percentage; }
    bool isPassed() const { return m_isPassed; }
    QDateTime getAttemptDate() const { return m_attemptDate; }
    int getTimeTakenSeconds() const { return m_timeTakenSeconds; }
    int getProctorWarnings() const { return m_proctorWarnings; }
    double getIntegrityScore() const { return m_integrityScore; }

    // Formatted helpers
    QString getFormattedDate() const { return m_attemptDate.toString("yyyy-MM-dd hh:mm AP"); }
    QString getFormattedTimeTaken() const;
    QString getStatusString() const { return m_isPassed ? "PASSED" : "FAILED"; }

    // Setters
    void setId(int id) { m_id = id; }
    void setStudentId(int id) { m_studentId = id; }
    void setStudentUsername(const QString& username) { m_studentUsername = username; }
    void setStudentFullName(const QString& name) { m_studentFullName = name; }
    void setQuizId(int id) { m_quizId = id; }
    void setQuizTitle(const QString& title) { m_quizTitle = title; }
    void setTotalQuestions(int count) { m_totalQuestions = count; }
    void setCorrectAnswers(int count) { m_correctAnswers = count; }
    void setIncorrectAnswers(int count) { m_incorrectAnswers = count; }
    void setUnansweredQuestions(int count) { m_unansweredQuestions = count; }
    void setTotalMarks(int marks) { m_totalMarks = marks; }
    void setMarksObtained(int marks) { m_marksObtained = marks; }
    void setPercentage(double pct) { m_percentage = pct; }
    void setPassed(bool passed) { m_isPassed = passed; }
    void setAttemptDate(const QDateTime& dt) { m_attemptDate = dt; }
    void setTimeTakenSeconds(int secs) { m_timeTakenSeconds = secs; }
    void setProctorWarnings(int warnings) { m_proctorWarnings = warnings; }
    void setIntegrityScore(double score) { m_integrityScore = score; }
};

#endif // RESULT_H
