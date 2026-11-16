#include "models/Quiz.h"

Quiz::Quiz()
    : m_id(0), m_title(""), m_category("General"), m_description(""),
      m_durationMinutes(10), m_passPercentage(50.0), m_totalMarks(0), m_createdBy("") {
}

Quiz::Quiz(int id, const QString& title, const QString& category,
           const QString& description, int durationMinutes,
           double passPercentage, int totalMarks, const QString& createdBy)
    : m_id(id), m_title(title.trimmed()), m_category(category.trimmed()),
      m_description(description.trimmed()),
      m_durationMinutes(durationMinutes > 0 ? durationMinutes : 10),
      m_passPercentage((passPercentage >= 0.0 && passPercentage <= 100.0) ? passPercentage : 50.0),
      m_totalMarks(totalMarks), m_createdBy(createdBy.trimmed()) {
}

void Quiz::setQuestions(const std::vector<Question>& questions) {
    m_questions = questions;
    recalculateTotalMarks();
}

void Quiz::addQuestion(const Question& question) {
    m_questions.push_back(question);
    recalculateTotalMarks();
}

void Quiz::removeQuestion(int questionId) {
    for (auto it = m_questions.begin(); it != m_questions.end(); ++it) {
        if (it->getId() == questionId) {
            m_questions.erase(it);
            break;
        }
    }
    recalculateTotalMarks();
}

const Question* Quiz::getQuestionAt(int index) const {
    if (index >= 0 && index < static_cast<int>(m_questions.size())) {
        return &m_questions[static_cast<size_t>(index)];
    }
    return nullptr;
}

#include "core/QuestionBank.h"

void Quiz::recalculateTotalMarks() {
    int total = 0;
    for (const auto& q : m_questions) {
        total += q.getMarks();
    }
    m_totalMarks = total;
}

void Quiz::randomizeQuestions(bool shuffleOptions) {
    if (m_questions.empty()) return;
    RandomizedQuestionSelector selector(shuffleOptions);
    m_questions = selector.selectQuestions(m_questions, m_questions.size());
    recalculateTotalMarks();
}

bool Quiz::isValid() const {
    return !m_title.trimmed().isEmpty() && m_durationMinutes > 0 && m_passPercentage >= 0.0;
}
