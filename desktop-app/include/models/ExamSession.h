#ifndef EXAM_SESSION_H
#define EXAM_SESSION_H

#include "models/Quiz.h"
#include <map>
#include <set>
#include <memory>

/**
 * @file ExamSession.h
 * @brief Manages runtime state of an active examination session.
 * 
 * Demonstrates:
 * - Encapsulation: Protects student's in-progress answers and timing.
 * - Cohesion: Keeps all session-specific logic decoupled from UI widgets.
 */
class ExamSession {
private:
    int m_studentId;
    Quiz m_quiz;
    std::map<int, int> m_userAnswers; // questionIndex -> selectedOptionIndex (0-3), or -1 if unselected
    std::set<int> m_markedForReview;   // set of questionIndices marked for review
    int m_currentQuestionIndex;
    int m_remainingSeconds;
    int m_totalDurationSeconds;
    bool m_isSubmitted;
    int m_proctorWarnings;
    double m_integrityScore;

public:
    ExamSession(int studentId, const Quiz& quiz);

    // Navigation and answering
    int getCurrentQuestionIndex() const { return m_currentQuestionIndex; }
    void setCurrentQuestionIndex(int index);

    void selectAnswer(int questionIndex, int optionIndex);
    void clearAnswer(int questionIndex);
    int getSelectedAnswer(int questionIndex) const;
    bool isAnswered(int questionIndex) const;

    void toggleMarkForReview(int questionIndex);
    bool isMarkedForReview(int questionIndex) const;

    // Statistics
    int getTotalQuestions() const { return m_quiz.getQuestionCount(); }
    int getAnsweredCount() const;
    int getUnansweredCount() const;
    int getMarkedForReviewCount() const { return static_cast<int>(m_markedForReview.size()); }

    // Anti-Cheating & Proctoring
    int getProctorWarnings() const { return m_proctorWarnings; }
    void addProctorWarning() { m_proctorWarnings++; m_integrityScore = (100.0 - m_proctorWarnings * 12.0 > 0.0) ? (100.0 - m_proctorWarnings * 12.0) : 0.0; }
    double getIntegrityScore() const { return m_integrityScore; }

    // Timing
    int getRemainingSeconds() const { return m_remainingSeconds; }
    int getTotalDurationSeconds() const { return m_totalDurationSeconds; }
    int getElapsedTimeSeconds() const { return m_totalDurationSeconds - m_remainingSeconds; }
    void decrementTimer();
    bool isTimeExpired() const { return m_remainingSeconds <= 0; }

    // Submission
    bool isSubmitted() const { return m_isSubmitted; }
    void setSubmitted(bool submitted) { m_isSubmitted = submitted; }

    // References
    const Quiz& getQuiz() const { return m_quiz; }
    const Question* getCurrentQuestion() const { return m_quiz.getQuestionAt(m_currentQuestionIndex); }
    int getStudentId() const { return m_studentId; }
};

using ExamSessionPtr = std::shared_ptr<ExamSession>;

#endif // EXAM_SESSION_H
