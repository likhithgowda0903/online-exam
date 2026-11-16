#ifndef QUESTION_BANK_H
#define QUESTION_BANK_H

#include "models/Question.h"
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <numeric>

/**
 * @file QuestionBank.h
 * @brief Strategy pattern interface and concrete engines for dynamic question
 *        randomization and rotating question pools across exam submissions.
 * 
 * Demonstrates Modern C++17 & OOP concepts:
 * - Strategy Design Pattern: Polymorphic question selection algorithms.
 * - STL Algorithms: std::shuffle, std::sample, std::transform.
 * - Standard C++17 <random> engine: std::mt19937 seeded with std::random_device.
 * - Encapsulation: Question option shuffling without leaking the answer key.
 */

// Abstract Question Selector Strategy (Open/Closed Principle)
class IQuestionSelector {
public:
    virtual ~IQuestionSelector() = default;

    /**
     * @brief Selects and orders a subset of questions from a master question pool.
     * @param pool The master bank of questions for the quiz.
     * @param count The desired number of questions to select.
     * @return A newly randomized and prepared vector of Question objects.
     */
    virtual std::vector<Question> selectQuestions(const std::vector<Question>& pool, size_t count) = 0;
};

// Concrete Strategy: Modern C++ Mersenne-Twister Randomizer
class RandomizedQuestionSelector : public IQuestionSelector {
private:
    bool m_shuffleOptions;

public:
    explicit RandomizedQuestionSelector(bool shuffleOptions = true)
        : m_shuffleOptions(shuffleOptions) {}
    ~RandomizedQuestionSelector() override = default;

    std::vector<Question> selectQuestions(const std::vector<Question>& pool, size_t count) override;

    /**
     * @brief Randomizes the 4 options of a question and updates the correct answer index.
     * @param q The Question object to shuffle.
     * @return Modified Question with shuffled options and remapped correct index.
     */
    static Question shuffleQuestionOptions(const Question& q);
};

// Question Bank Manager holding subject question repositories
class QuestionBank {
private:
    int m_quizId;
    std::vector<Question> m_masterPool;
    std::shared_ptr<IQuestionSelector> m_selector;
    int m_submissionAttemptCount;

public:
    QuestionBank(int quizId = 0, std::shared_ptr<IQuestionSelector> selector = nullptr);

    void setQuizId(int quizId) { m_quizId = quizId; }
    int getQuizId() const { return m_quizId; }

    void addQuestionToPool(const Question& q) { m_masterPool.push_back(q); }
    void setMasterPool(const std::vector<Question>& pool) { m_masterPool = pool; }
    const std::vector<Question>& getMasterPool() const { return m_masterPool; }
    size_t getPoolSize() const { return m_masterPool.size(); }

    void setSelector(std::shared_ptr<IQuestionSelector> selector) { m_selector = selector; }

    /**
     * @brief Generates a fresh, randomized question set for the next exam attempt.
     *        Called every time an exam submission completes.
     * @param questionCount Number of questions to draw.
     * @return Vector of freshly selected and randomized questions.
     */
    std::vector<Question> generateNextExamSet(size_t questionCount);

    void onExamSubmitted();
    int getAttemptCount() const { return m_submissionAttemptCount; }
};

using QuestionBankPtr = std::shared_ptr<QuestionBank>;

#endif // QUESTION_BANK_H
