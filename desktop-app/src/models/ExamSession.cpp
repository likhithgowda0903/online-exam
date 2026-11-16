#include "models/ExamSession.h"

ExamSession::ExamSession(int studentId, const Quiz& quiz)
    : m_studentId(studentId), m_quiz(quiz), m_currentQuestionIndex(0),
      m_totalDurationSeconds(quiz.getDurationMinutes() * 60),
      m_remainingSeconds(quiz.getDurationMinutes() * 60),
      m_isSubmitted(false),
      m_proctorWarnings(0),
      m_integrityScore(100.0) {
    // Initialize all answers to -1 (unanswered)
    for (int i = 0; i < m_quiz.getQuestionCount(); ++i) {
        m_userAnswers[i] = -1;
    }
}

void ExamSession::setCurrentQuestionIndex(int index) {
    if (index >= 0 && index < m_quiz.getQuestionCount()) {
        m_currentQuestionIndex = index;
    }
}

void ExamSession::selectAnswer(int questionIndex, int optionIndex) {
    if (questionIndex >= 0 && questionIndex < m_quiz.getQuestionCount()) {
        if (optionIndex >= 0 && optionIndex < 4) {
            m_userAnswers[questionIndex] = optionIndex;
        }
    }
}

void ExamSession::clearAnswer(int questionIndex) {
    if (questionIndex >= 0 && questionIndex < m_quiz.getQuestionCount()) {
        m_userAnswers[questionIndex] = -1;
    }
}

int ExamSession::getSelectedAnswer(int questionIndex) const {
    auto it = m_userAnswers.find(questionIndex);
    if (it != m_userAnswers.end()) {
        return it->second;
    }
    return -1;
}

bool ExamSession::isAnswered(int questionIndex) const {
    return getSelectedAnswer(questionIndex) != -1;
}

void ExamSession::toggleMarkForReview(int questionIndex) {
    if (m_markedForReview.find(questionIndex) != m_markedForReview.end()) {
        m_markedForReview.erase(questionIndex);
    } else {
        m_markedForReview.insert(questionIndex);
    }
}

bool ExamSession::isMarkedForReview(int questionIndex) const {
    return m_markedForReview.find(questionIndex) != m_markedForReview.end();
}

int ExamSession::getAnsweredCount() const {
    int count = 0;
    for (const auto& pair : m_userAnswers) {
        if (pair.second != -1) {
            count++;
        }
    }
    return count;
}

int ExamSession::getUnansweredCount() const {
    return getTotalQuestions() - getAnsweredCount();
}

void ExamSession::decrementTimer() {
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
    }
}
