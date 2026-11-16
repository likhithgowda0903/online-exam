#ifndef QUESTION_H
#define QUESTION_H

#include <QString>
#include <QStringList>
#include <vector>

/**
 * @file Question.h
 * @brief Question model representing a multiple-choice question.
 * 
 * Demonstrates Encapsulation:
 * - Question text, 4 options, correct answer index, and marks are kept private.
 * - Public interface verifies choices without exposing the internal answer directly to students before submission.
 */
class Question {
private:
    int m_id;
    int m_quizId;
    QString m_questionText;
    QStringList m_options; // exactly 4 options: [0] = A, [1] = B, [2] = C, [3] = D
    int m_correctOptionIndex; // 0, 1, 2, or 3
    int m_marks;
    QString m_explanation;

public:
    Question();
    Question(int id, int quizId, const QString& questionText,
             const QStringList& options, int correctOptionIndex,
             int marks = 1, const QString& explanation = "");

    // Getters
    int getId() const { return m_id; }
    int getQuizId() const { return m_quizId; }
    QString getQuestionText() const { return m_questionText; }
    QStringList getOptions() const { return m_options; }
    QString getOption(int index) const;
    int getCorrectOptionIndex() const { return m_correctOptionIndex; }
    int getMarks() const { return m_marks; }
    QString getExplanation() const { return m_explanation; }

    // Setters
    void setId(int id) { m_id = id; }
    void setQuizId(int quizId) { m_quizId = quizId; }
    void setQuestionText(const QString& text) { m_questionText = text.trimmed(); }
    void setOptions(const QStringList& options);
    void setOption(int index, const QString& text);
    void setCorrectOptionIndex(int index);
    void setMarks(int marks) { m_marks = marks > 0 ? marks : 1; }
    void setExplanation(const QString& exp) { m_explanation = exp.trimmed(); }

    // Verification methods
    bool isCorrect(int selectedIndex) const {
        return selectedIndex == m_correctOptionIndex;
    }

    bool isValid() const;
};

#endif // QUESTION_H
