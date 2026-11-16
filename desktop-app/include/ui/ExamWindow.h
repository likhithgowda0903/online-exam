#ifndef EXAM_WINDOW_H
#define EXAM_WINDOW_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <vector>
#include "models/ExamSession.h"
#include "core/ExamTimer.h"
#include "models/Result.h"

/**
 * @brief Custom Circular Progress Widget showing "Answered / Total" with circular stroke
 */
class CircularProgressWidget : public QWidget {
    Q_OBJECT
private:
    int m_current;
    int m_total;

public:
    explicit CircularProgressWidget(QWidget* parent = nullptr)
        : QWidget(parent), m_current(0), m_total(10) {
        setFixedSize(160, 160);
    }

    void setProgress(int current, int total) {
        m_current = current;
        m_total = total > 0 ? total : 1;
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        int side = qMin(width(), height());
        QRectF rect((width() - side) / 2.0 + 10, (height() - side) / 2.0 + 10, side - 20, side - 20);

        // Background Track
        QPen trackPen(QColor("#e5e7eb"), 12, Qt::SolidLine, Qt::RoundCap);
        p.setPen(trackPen);
        p.drawEllipse(rect);

        // Progress Arc in Forest Green (#233d32)
        QPen progPen(QColor("#233d32"), 12, Qt::SolidLine, Qt::RoundCap);
        p.setPen(progPen);

        double fraction = static_cast<double>(m_current) / m_total;
        int spanAngle = static_cast<int>(-fraction * 360 * 16);
        int startAngle = 90 * 16;
        p.drawArc(rect, startAngle, spanAngle);

        // Center Text e.g. "9/10"
        p.setPen(QColor("#111827"));
        QFont f = font();
        f.setPointSize(22);
        f.setBold(true);
        p.setFont(f);
        p.drawText(rect, Qt::AlignCenter, QString("%1/%2").arg(m_current).arg(m_total));
    }
};

/**
 * @file ExamWindow.h
 * @brief Modern Examination Window matching the clean white card & circular gauge design.
 */
class ExamWindow : public QDialog {
    Q_OBJECT

private:
    ExamSessionPtr m_session;
    ExamTimer* m_timer;
    bool m_submitted;
    Result m_finalResult;

    // Header Widgets
    QLabel* m_timerValueLabel;
    QPushButton* m_submitButton;

    // Question Body Widgets
    QLabel* m_questionCounterLabel;
    QLabel* m_questionPromptLabel;
    std::vector<QPushButton*> m_optionButtons; // 2x2 Grid: Option A, B, C, D
    CircularProgressWidget* m_circularGauge;

    // Bottom Footer Pagination
    QPushButton* m_prevButton;
    QPushButton* m_nextButton;
    std::vector<QPushButton*> m_pageNumberButtons;

    void setupUI();
    void renderCurrentQuestion();
    void updatePagination();

public:
    explicit ExamWindow(ExamSessionPtr session, QWidget* parent = nullptr);
    ~ExamWindow() override;

    bool isSubmitted() const { return m_submitted; }
    Result getFinalResult() const { return m_finalResult; }

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onOptionClicked(int optionIndex);
    void onPrevClicked();
    void onNextClicked();
    void onPageNumberClicked(int questionIndex);
    void onSubmitClicked();

    // Timer slots
    void onTimerTick(int remainingSeconds, const QString& formattedTime);
    void onTimerWarning(int remainingSeconds, const QString& formattedTime);
    void onTimerTimeout();
};

#endif // EXAM_WINDOW_H
