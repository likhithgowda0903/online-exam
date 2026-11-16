#include "core/QuestionBank.h"
#include <chrono>

std::vector<Question> RandomizedQuestionSelector::selectQuestions(const std::vector<Question>& pool, size_t count) {
    if (pool.empty()) {
        return {};
    }

    std::vector<Question> copyPool = pool;

    // Modern C++17 Mersenne-Twister engine seeded with random_device and high-resolution clock
    std::random_device rd;
    std::mt19937_64 rng(static_cast<uint64_t>(rd()) ^ 
                        static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    // Shuffle the entire question pool to guarantee non-repeating dynamic selection
    std::shuffle(copyPool.begin(), copyPool.end(), rng);

    // Take up to 'count' questions
    size_t takeCount = std::min(count, copyPool.size());
    std::vector<Question> selected(copyPool.begin(), copyPool.begin() + takeCount);

    // Optionally shuffle the choices (A, B, C, D) within each individual question
    if (m_shuffleOptions) {
        for (auto& q : selected) {
            q = shuffleQuestionOptions(q);
        }
    }

    return selected;
}

Question RandomizedQuestionSelector::shuffleQuestionOptions(const Question& q) {
    QStringList origOpts = q.getOptions();
    if (origOpts.size() != 4) {
        return q; // Only 4-option MCQs are shuffled
    }

    int origCorrect = q.getCorrectOptionIndex();

    // Map indices: 0 -> A, 1 -> B, 2 -> C, 3 -> D
    std::vector<int> indices = {0, 1, 2, 3};

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    QStringList newOpts;
    int newCorrect = 0;

    for (size_t i = 0; i < indices.size(); ++i) {
        int oldIdx = indices[i];
        newOpts.append(origOpts[oldIdx]);
        if (oldIdx == origCorrect) {
            newCorrect = static_cast<int>(i);
        }
    }

    return Question(
        q.getId(),
        q.getQuizId(),
        q.getQuestionText(),
        newOpts,
        newCorrect,
        q.getMarks(),
        q.getExplanation()
    );
}

QuestionBank::QuestionBank(int quizId, std::shared_ptr<IQuestionSelector> selector)
    : m_quizId(quizId), m_selector(selector), m_submissionAttemptCount(0) {
    if (!m_selector) {
        m_selector = std::make_shared<RandomizedQuestionSelector>(true);
    }
}

std::vector<Question> QuestionBank::generateNextExamSet(size_t questionCount) {
    if (!m_selector) {
        m_selector = std::make_shared<RandomizedQuestionSelector>(true);
    }
    return m_selector->selectQuestions(m_masterPool, questionCount);
}

void QuestionBank::onExamSubmitted() {
    m_submissionAttemptCount++;
}
