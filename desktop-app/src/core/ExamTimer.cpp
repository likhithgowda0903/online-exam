#include "core/ExamTimer.h"

ExamTimer::ExamTimer(QObject* parent)
    : QObject(parent), m_timer(new QTimer(this)),
      m_remainingSeconds(0), m_totalDurationSeconds(0),
      m_warningThresholdSeconds(120), m_hasEmittedWarning(false) {
    connect(m_timer, &QTimer::timeout, this, &ExamTimer::onTimerTick);
}

ExamTimer::~ExamTimer() {
    stop();
}

void ExamTimer::start(int durationSeconds, int warningThresholdSeconds) {
    m_totalDurationSeconds = durationSeconds > 0 ? durationSeconds : 600;
    m_remainingSeconds = m_totalDurationSeconds;
    m_warningThresholdSeconds = warningThresholdSeconds > 0 ? warningThresholdSeconds : 120;
    m_hasEmittedWarning = false;

    m_timer->start(1000); // 1-second interval
    emit tick(m_remainingSeconds, formatTime(m_remainingSeconds));
}

void ExamTimer::pause() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void ExamTimer::resume() {
    if (!m_timer->isActive() && m_remainingSeconds > 0) {
        m_timer->start(1000);
    }
}

void ExamTimer::stop() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

QString ExamTimer::formatTime(int totalSeconds) {
    if (totalSeconds < 0) totalSeconds = 0;
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));
}

void ExamTimer::onTimerTick() {
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        QString formatted = formatTime(m_remainingSeconds);
        emit tick(m_remainingSeconds, formatted);

        if (!m_hasEmittedWarning && m_remainingSeconds <= m_warningThresholdSeconds && m_remainingSeconds > 0) {
            m_hasEmittedWarning = true;
            emit warningLowTime(m_remainingSeconds, formatted);
        }
    }

    if (m_remainingSeconds <= 0) {
        stop();
        emit timeout();
    }
}
