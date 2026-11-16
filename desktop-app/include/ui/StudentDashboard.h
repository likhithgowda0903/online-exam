#ifndef STUDENT_DASHBOARD_H
#define STUDENT_DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <vector>
#include "models/Student.h"
#include "models/Quiz.h"
#include "models/Result.h"

/**
 * @file StudentDashboard.h
 * @brief Student portal displaying available exams, past attempts, and academic metrics.
 */
class StudentDashboard : public QWidget {
    Q_OBJECT

private:
    StudentPtr m_student;

    // Stat cards
    QLabel* m_welcomeLabel;
    QLabel* m_studentIdBadge;
    QLabel* m_examsTakenStat;
    QLabel* m_averageScoreStat;
    QLabel* m_passedExamsStat;

    // Tables
    QTableWidget* m_availableQuizzesTable;
    QTableWidget* m_historyTable;

    std::vector<Quiz> m_availableQuizzes;
    std::vector<Result> m_studentResults;

    void setupUI();

public:
    explicit StudentDashboard(StudentPtr student, QWidget* parent = nullptr);
    ~StudentDashboard() override = default;

    void refreshData();

signals:
    void logoutRequested();

private slots:
    void handleOpenDatabase();
    void onStartExamClicked(int quizId);
    void onViewResultClicked(int resultId);
};

#endif // STUDENT_DASHBOARD_H
