#include "models/Result.h"

Result::Result()
    : m_id(0), m_studentId(0), m_studentUsername(""), m_studentFullName(""),
      m_quizId(0), m_quizTitle(""), m_totalQuestions(0), m_correctAnswers(0),
      m_incorrectAnswers(0), m_unansweredQuestions(0), m_totalMarks(0),
      m_marksObtained(0), m_percentage(0.0), m_isPassed(false),
      m_attemptDate(QDateTime::currentDateTime()), m_timeTakenSeconds(0),
      m_proctorWarnings(0), m_integrityScore(100.0) {
}

Result::Result(int id, int studentId, int quizId,
               int totalQuestions, int correctAnswers, int incorrectAnswers, int unansweredQuestions,
               int totalMarks, int marksObtained, double percentage, bool isPassed,
               const QDateTime& attemptDate, int timeTakenSeconds,
               const QString& studentUsername, const QString& studentFullName,
               const QString& quizTitle, int proctorWarnings, double integrityScore)
    : m_id(id), m_studentId(studentId), m_studentUsername(studentUsername),
      m_studentFullName(studentFullName), m_quizId(quizId), m_quizTitle(quizTitle),
      m_totalQuestions(totalQuestions), m_correctAnswers(correctAnswers),
      m_incorrectAnswers(incorrectAnswers), m_unansweredQuestions(unansweredQuestions),
      m_totalMarks(totalMarks), m_marksObtained(marksObtained),
      m_percentage(percentage), m_isPassed(isPassed),
      m_attemptDate(attemptDate), m_timeTakenSeconds(timeTakenSeconds),
      m_proctorWarnings(proctorWarnings), m_integrityScore(integrityScore) {
}

QString Result::getFormattedTimeTaken() const {
    int mins = m_timeTakenSeconds / 60;
    int secs = m_timeTakenSeconds % 60;
    return QString("%1m %2s").arg(mins).arg(secs, 2, 10, QChar('0'));
}
