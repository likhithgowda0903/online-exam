#ifndef EXAM_TIMER_H
#define EXAM_TIMER_H

#include <QObject>
#include <QTimer>
#include <QString>

/**
 * @file ExamTimer.h
 * @brief Object-Oriented Countdown Timer for online examinations.
 * 
 * Demonstrates:
 * - Event-driven architecture with Qt signals and slots.
 * - Encapsulation of time tracking, warning thresholds, and automatic expiration.
 */
class ExamTimer : public QObject {
    Q_OBJECT

private:
    QTimer* m_timer;
    int m_remainingSeconds;
    int m_totalDurationSeconds;
    int m_warningThresholdSeconds;
    bool m_hasEmittedWarning;

public:
    explicit ExamTimer(QObject* parent = nullptr);
    ~ExamTimer() override;

    void start(int durationSeconds, int warningThresholdSeconds = 120);
    void pause();
    void resume();
    void stop();

    int getRemainingSeconds() const { return m_remainingSeconds; }
    int getTotalDurationSeconds() const { return m_totalDurationSeconds; }
    int getElapsedSeconds() const { return m_totalDurationSeconds - m_remainingSeconds; }
    bool isRunning() const { return m_timer && m_timer->isActive(); }
    bool isLowTime() const { return m_remainingSeconds <= m_warningThresholdSeconds && m_remainingSeconds > 0; }

    static QString formatTime(int totalSeconds);

signals:
    void tick(int remainingSeconds, const QString& formattedTime);
    void warningLowTime(int remainingSeconds, const QString& formattedTime);
    void timeout();

private slots:
    void onTimerTick();
};

#endif // EXAM_TIMER_H
