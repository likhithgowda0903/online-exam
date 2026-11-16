#ifndef QUIZ_H
#define QUIZ_H

#include "models/Question.h"
#include <QString>
#include <vector>
#include <memory>

/**
 * @file Quiz.h
 * @brief Quiz model representing an examination package.
 * 
 * Demonstrates:
 * - Aggregation: A Quiz aggregates multiple Question objects.
 * - Encapsulation: Protects quiz parameters such as pass marks and duration.
 */
class Quiz {
private:
    int m_id;
    QString m_title;
    QString m_category;
    QString m_description;
    int m_durationMinutes;
    double m_passPercentage;
    int m_totalMarks;
    QString m_createdBy;
    std::vector<Question> m_questions;

public:
    Quiz();
    Quiz(int id, const QString& title, const QString& category,
         const QString& description, int durationMinutes,
         double passPercentage = 50.0, int totalMarks = 0,
         const QString& createdBy = "");

    // Getters
    int getId() const { return m_id; }
    QString getTitle() const { return m_title; }
    QString getCategory() const { return m_category; }
    QString getDescription() const { return m_description; }
    int getDurationMinutes() const { return m_durationMinutes; }
    double getPassPercentage() const { return m_passPercentage; }
    int getTotalMarks() const { return m_totalMarks; }
    QString getCreatedBy() const { return m_createdBy; }
    const std::vector<Question>& getQuestions() const { return m_questions; }
    int getQuestionCount() const { return static_cast<int>(m_questions.size()); }

    // Setters
    void setId(int id) { m_id = id; }
    void setTitle(const QString& title) { m_title = title.trimmed(); }
    void setCategory(const QString& category) { m_category = category.trimmed(); }
    void setDescription(const QString& desc) { m_description = desc.trimmed(); }
    void setDurationMinutes(int minutes) { m_durationMinutes = minutes > 0 ? minutes : 10; }
    void setPassPercentage(double pct) { m_passPercentage = (pct >= 0.0 && pct <= 100.0) ? pct : 50.0; }
    void setTotalMarks(int marks) { m_totalMarks = marks; }
    void setCreatedBy(const QString& creator) { m_createdBy = creator.trimmed(); }

    // Question collection management
    void setQuestions(const std::vector<Question>& questions);
    void addQuestion(const Question& question);
    void removeQuestion(int questionId);
    const Question* getQuestionAt(int index) const;
    void recalculateTotalMarks();
    void randomizeQuestions(bool shuffleOptions = true);

    bool isValid() const;
};

using QuizPtr = std::shared_ptr<Quiz>;

#endif // QUIZ_H
