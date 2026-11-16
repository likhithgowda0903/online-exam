#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <vector>
#include <optional>
#include <memory>
#include "models/User.h"
#include "models/Student.h"
#include "models/Admin.h"
#include "models/Quiz.h"
#include "models/Question.h"
#include "models/Result.h"

class QWidget;

/**
 * @file DatabaseManager.h
 * @brief Singleton Database Manager encapsulating all SQLite database operations.
 * 
 * Demonstrates:
 * - Singleton Design Pattern: Single centralized access point to SQLite database.
 * - Encapsulation & Security: Parameterized prepared queries preventing SQL injections.
 * - Persistence: Handles tables creation, seed data generation, and full CRUD.
 */
class DatabaseManager {
private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    QString m_dbPath;
    bool m_isInitialized;

    bool createTables();

public:
    static DatabaseManager& instance();

    bool initDatabase(const QString& dbPath = "online_exam.db");
    bool isConnected() const;
    void closeDatabase();

    // SQLite External Viewer Helper (DB Browser for SQLite via QProcess)
    QString getDatabasePath() const;
    QString findDbBrowserExecutable() const;
    bool openInDbBrowser(QWidget* parentWidget = nullptr);

    // Default Seed Data Generator
    void seedDefaultData();

    // User Management
    bool registerStudent(const QString& username, const QString& password, 
                         const QString& fullName, const QString& email, 
                         const QString& studentId, QString& errorMsg);
    bool registerAdmin(const QString& username, const QString& password, 
                       const QString& fullName, const QString& email, 
                       const QString& adminId, QString& errorMsg);
    UserPtr authenticateUser(const QString& username, const QString& password, QString& errorMsg);
    bool isUsernameTaken(const QString& username);
    bool isEmailTaken(const QString& email);
    bool isStudentIdTaken(const QString& studentId);
    std::vector<Student> getAllStudents();
    int getTotalStudentsCount();
    bool deleteStudent(int userId);

    // Quiz Management
    int saveQuiz(const Quiz& quiz, QString& errorMsg);
    bool updateQuiz(const Quiz& quiz, QString& errorMsg);
    bool deleteQuiz(int quizId);
    std::vector<Quiz> getAllQuizzes(bool loadQuestions = true);
    std::vector<Quiz> getQuizzesByAdmin(const QString& adminId, bool loadQuestions = true);
    std::optional<Quiz> getQuizById(int quizId, bool loadQuestions = true);
    int getTotalQuizzesCount();
    int getTotalQuizzesCountByAdmin(const QString& adminId);

    // Question Management
    int saveQuestion(const Question& question, QString& errorMsg);
    bool updateQuestion(const Question& question, QString& errorMsg);
    bool deleteQuestion(int questionId);
    std::vector<Question> getQuestionsForQuiz(int quizId);
    std::vector<Question> getDynamicQuestionsForQuiz(int quizId, int count = 20, bool shuffleOptions = true);

    // Result Management
    int saveResult(const Result& result, QString& errorMsg);
    std::vector<Result> getResultsForStudent(int studentId);
    std::vector<Result> getAllResults();
    std::optional<Result> getResultById(int resultId);
    int getTotalExamsCompletedCount();
    double getGlobalAverageScore();
    double getStudentAverageScore(int studentId);
    int getStudentCompletedExamsCount(int studentId);
    int getStudentPassedExamsCount(int studentId);
};

#endif // DATABASE_MANAGER_H
