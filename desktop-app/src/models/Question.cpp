#include "models/Question.h"

Question::Question()
    : m_id(0), m_quizId(0), m_questionText(""), m_options{"", "", "", ""},
      m_correctOptionIndex(0), m_marks(1), m_explanation("") {
}

Question::Question(int id, int quizId, const QString& questionText,
                   const QStringList& options, int correctOptionIndex,
                   int marks, const QString& explanation)
    : m_id(id), m_quizId(quizId), m_questionText(questionText.trimmed()),
      m_correctOptionIndex(correctOptionIndex), m_marks(marks > 0 ? marks : 1),
      m_explanation(explanation.trimmed()) {
    setOptions(options);
}

QString Question::getOption(int index) const {
    if (index >= 0 && index < m_options.size()) {
        return m_options[index];
    }
    return "";
}

void Question::setOptions(const QStringList& options) {
    m_options = options;
    while (m_options.size() < 4) {
        m_options.append("");
    }
    while (m_options.size() > 4) {
        m_options.removeLast();
    }
}

void Question::setOption(int index, const QString& text) {
    if (index >= 0 && index < 4) {
        while (m_options.size() <= index) {
            m_options.append("");
        }
        m_options[index] = text.trimmed();
    }
}

void Question::setCorrectOptionIndex(int index) {
    if (index >= 0 && index < 4) {
        m_correctOptionIndex = index;
    }
}

bool Question::isValid() const {
    if (m_questionText.trimmed().isEmpty()) return false;
    if (m_options.size() != 4) return false;
    for (const auto& opt : m_options) {
        if (opt.trimmed().isEmpty()) return false;
    }
    if (m_correctOptionIndex < 0 || m_correctOptionIndex > 3) return false;
    if (m_marks <= 0) return false;
    return true;
}
