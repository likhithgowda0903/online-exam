#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <vector>
#include "models/Admin.h"
#include "models/Quiz.h"
#include "models/Student.h"
#include "models/Result.h"

/**
 * @file AdminDashboard.h
 * @brief Administrator portal for exam management, student oversight, and global analytics.
 */
class AdminDashboard : public QWidget {
    Q_OBJECT

private:
    AdminPtr m_admin;

    // Analytics Labels
    QLabel* m_totalStudentsStat;
    QLabel* m_totalQuizzesStat;
    QLabel* m_completedExamsStat;
    QLabel* m_globalAverageStat;

    // Tabs & Tables
    QTabWidget* m_tabWidget;
    QTableWidget* m_quizzesTable;
    QTableWidget* m_studentsTable;
    QTableWidget* m_resultsTable;

    std::vector<Quiz> m_quizzesList;
    std::vector<Student> m_studentsList;
    std::vector<Result> m_resultsList;

    void setupUI();
    QWidget* createQuizzesTab();
    QWidget* createStudentsTab();
    QWidget* createResultsTab();

public:
    explicit AdminDashboard(AdminPtr admin, QWidget* parent = nullptr);
    ~AdminDashboard() override = default;

    void refreshData();

signals:
    void logoutRequested();

private slots:
    void handleOpenDatabase();
    void handleCreateQuiz();
    void handleEditQuiz();
    void handleDeleteQuiz();
    void handleDeleteStudent();
    void handleViewResult();
};

#endif // ADMIN_DASHBOARD_H
