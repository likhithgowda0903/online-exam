#include "ui/ExamWindow.h"
#include "core/Evaluator.h"
#include "database/DatabaseManager.h"
#include "ui/ResultDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFrame>

ExamWindow::ExamWindow(ExamSessionPtr session, QWidget* parent)
    : QDialog(parent), m_session(session), m_timer(new ExamTimer(this)),
      m_submitted(false) {
    setWindowTitle("Online Examination - " + session->getQuiz().getTitle());
    setMinimumSize(980, 640);
    resize(1060, 680);

    // Light sage green background for the outer window matching the design
    setStyleSheet(R"(
        QDialog {
            background-color: #d4e4de;
        }
    )");

    setupUI();

    connect(m_timer, &ExamTimer::tick, this, &ExamWindow::onTimerTick);
    connect(m_timer, &ExamTimer::warningLowTime, this, &ExamWindow::onTimerWarning);
    connect(m_timer, &ExamTimer::timeout, this, &ExamWindow::onTimerTimeout);

    m_timer->start(m_session->getTotalDurationSeconds(), 120);

    renderCurrentQuestion();
    updatePagination();
}

ExamWindow::~ExamWindow() {
    if (m_timer) {
        m_timer->stop();
    }
}

void ExamWindow::setupUI() {
    auto outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(28, 28, 28, 28);

    // Main Pure White Card Container
    auto mainCard = new QFrame(this);
    mainCard->setStyleSheet(R"(
        QFrame#mainCard {
            background-color: #ffffff;
            border-radius: 26px;
            border: 1px solid #e5e7eb;
        }
    )");
    mainCard->setObjectName("mainCard");

    auto cardLayout = new QVBoxLayout(mainCard);
    cardLayout->setContentsMargins(40, 36, 40, 36);
    cardLayout->setSpacing(24);

    // 1. Top Header Row: Clock Icon + "Time remaining" + Time Label on Left; Dark Pine "Submit" Button on Right
    auto topHeaderLayout = new QHBoxLayout();

    auto timerLeftLayout = new QHBoxLayout();
    timerLeftLayout->setSpacing(14);

    auto clockIconLabel = new QLabel(mainCard);
    clockIconLabel->setText("🕒");
    clockIconLabel->setStyleSheet("font-size: 26px; color: #111827;");
    timerLeftLayout->addWidget(clockIconLabel);

    auto timerTextGroup = new QVBoxLayout();
    timerTextGroup->setSpacing(2);

    auto timerSubLabel = new QLabel("Time remaining", mainCard);
    timerSubLabel->setStyleSheet("font-size: 11px; color: #6b7280; font-weight: 500;");

    m_timerValueLabel = new QLabel("14 : 44 : 00", mainCard);
    m_timerValueLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #111827; letter-spacing: 1px; font-family: Segoe UI, sans-serif;");

    timerTextGroup->addWidget(timerSubLabel);
    timerTextGroup->addWidget(m_timerValueLabel);
    timerLeftLayout->addLayout(timerTextGroup);

    topHeaderLayout->addLayout(timerLeftLayout);
    topHeaderLayout->addStretch();

    // Dark Forest/Pine Green Submit Button
    m_submitButton = new QPushButton("Submit", mainCard);
    m_submitButton->setCursor(Qt::PointingHandCursor);
    m_submitButton->setStyleSheet(R"(
        QPushButton {
            background-color: #233d32;
            color: #ffffff;
            border: none;
            border-radius: 10px;
            padding: 10px 32px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton:hover {
            background-color: #1a2e26;
        }
        QPushButton:pressed {
            background-color: #12211b;
        }
    )");
    connect(m_submitButton, &QPushButton::clicked, this, &ExamWindow::onSubmitClicked);
    topHeaderLayout->addWidget(m_submitButton);

    cardLayout->addLayout(topHeaderLayout);

    // 2. Middle Body: Left (Question & 2x2 Options) + Right (Circular 9/10 Gauge)
    auto bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(36);

    auto questionSideLayout = new QVBoxLayout();
    questionSideLayout->setSpacing(14);

    m_questionCounterLabel = new QLabel("Question 1 of 10", mainCard);
    m_questionCounterLabel->setStyleSheet("font-size: 14px; color: #4b5563; font-weight: 600;");
    questionSideLayout->addWidget(m_questionCounterLabel);

    m_questionPromptLabel = new QLabel(mainCard);
    m_questionPromptLabel->setStyleSheet("font-size: 18px; font-weight: 700; color: #111827; line-height: 1.4;");
    m_questionPromptLabel->setWordWrap(true);
    questionSideLayout->addWidget(m_questionPromptLabel);

    // 2x2 Options Grid matching reference image
    auto optionsGrid = new QGridLayout();
    optionsGrid->setSpacing(16);

    m_optionButtons.clear();
    for (int i = 0; i < 4; ++i) {
        auto optBtn = new QPushButton(mainCard);
        optBtn->setCursor(Qt::PointingHandCursor);
        optBtn->setMinimumHeight(52);
        optBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #ffffff;
                border: 1.5px solid #e5e7eb;
                border-radius: 12px;
                padding: 10px 18px;
                font-size: 15px;
                font-weight: 700;
                color: #1f2937;
                text-align: left;
            }
            QPushButton:hover {
                border-color: #9ca3af;
                background-color: #f9fafb;
            }
        )");

        connect(optBtn, &QPushButton::clicked, [this, i]() {
            onOptionClicked(i);
        });

        m_optionButtons.push_back(optBtn);
        optionsGrid->addWidget(optBtn, i / 2, i % 2);
    }

    questionSideLayout->addLayout(optionsGrid);
    bodyLayout->addLayout(questionSideLayout, 1);

    // Right: Circular Progress Gauge (e.g. 9/10)
    m_circularGauge = new CircularProgressWidget(mainCard);
    bodyLayout->addWidget(m_circularGauge, 0, Qt::AlignCenter);

    cardLayout->addLayout(bodyLayout);

    // 3. Bottom Footer Pagination Bar: [ Prev ] [ 1 ] [ 2 ] ... [ 10 ] [ Next ]
    auto footerLayout = new QHBoxLayout();
    footerLayout->setAlignment(Qt::AlignCenter);
    footerLayout->setSpacing(10);

    m_prevButton = new QPushButton("Prev", mainCard);
    m_prevButton->setCursor(Qt::PointingHandCursor);
    m_prevButton->setFixedSize(70, 42);
    m_prevButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ffffff;
            border: 1.5px solid #e5e7eb;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 700;
            color: #1f2937;
        }
        QPushButton:hover:!disabled {
            border-color: #9ca3af;
            background-color: #f9fafb;
        }
        QPushButton:disabled {
            color: #d1d5db;
            border-color: #f3f4f6;
        }
    )");
    connect(m_prevButton, &QPushButton::clicked, this, &ExamWindow::onPrevClicked);
    footerLayout->addWidget(m_prevButton);

    m_pageNumberButtons.clear();
    int totalQ = m_session->getTotalQuestions();
    for (int i = 0; i < totalQ; ++i) {
        auto numBtn = new QPushButton(QString::number(i + 1), mainCard);
        numBtn->setCursor(Qt::PointingHandCursor);
        numBtn->setFixedSize(42, 42);

        connect(numBtn, &QPushButton::clicked, [this, i]() {
            onPageNumberClicked(i);
        });

        m_pageNumberButtons.push_back(numBtn);
        footerLayout->addWidget(numBtn);
    }

    m_nextButton = new QPushButton("Next", mainCard);
    m_nextButton->setCursor(Qt::PointingHandCursor);
    m_nextButton->setFixedSize(70, 42);
    m_nextButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ffffff;
            border: 1.5px solid #e5e7eb;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 700;
            color: #1f2937;
        }
        QPushButton:hover:!disabled {
            border-color: #9ca3af;
            background-color: #f9fafb;
        }
        QPushButton:disabled {
            color: #d1d5db;
            border-color: #f3f4f6;
        }
    )");
    connect(m_nextButton, &QPushButton::clicked, this, &ExamWindow::onNextClicked);
    footerLayout->addWidget(m_nextButton);

    cardLayout->addLayout(footerLayout);
    outerLayout->addWidget(mainCard);
}

void ExamWindow::renderCurrentQuestion() {
    int curIdx = m_session->getCurrentQuestionIndex();
    const Question* q = m_session->getCurrentQuestion();
    if (!q) return;

    m_questionCounterLabel->setText(QString("Question %1 of %2").arg(curIdx + 1).arg(m_session->getTotalQuestions()));
    m_questionPromptLabel->setText(q->getQuestionText());

    int selected = m_session->getSelectedAnswer(curIdx);

    // Update 2x2 Option Buttons
    for (int i = 0; i < 4; ++i) {
        auto btn = m_optionButtons[i];
        QString letter = QString(QChar('A' + i)) + ".";
        btn->setText(QString("  %1      %2").arg(letter).arg(q->getOption(i)));

        if (selected == i) {
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: #e9f2ee;
                    border: 2px solid #233d32;
                    border-radius: 12px;
                    padding: 10px 18px;
                    font-size: 15px;
                    font-weight: 700;
                    color: #111827;
                    text-align: left;
                }
            )");
        } else {
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: #ffffff;
                    border: 1.5px solid #e5e7eb;
                    border-radius: 12px;
                    padding: 10px 18px;
                    font-size: 15px;
                    font-weight: 700;
                    color: #1f2937;
                    text-align: left;
                }
                QPushButton:hover {
                    border-color: #9ca3af;
                    background-color: #f9fafb;
                }
            )");
        }
    }

    // Update Navigation Buttons
    m_prevButton->setEnabled(curIdx > 0);
    m_nextButton->setEnabled(curIdx < m_session->getTotalQuestions() - 1);

    updatePagination();
}

void ExamWindow::updatePagination() {
    int curIdx = m_session->getCurrentQuestionIndex();
    int totalQ = m_session->getTotalQuestions();
    int answeredCount = m_session->getAnsweredCount();

    // Update Circular Gauge
    m_circularGauge->setProgress(answeredCount, totalQ);

    // Update Bottom Page Number Buttons
    for (int i = 0; i < totalQ; ++i) {
        auto numBtn = m_pageNumberButtons[i];

        if (i == curIdx) {
            // Active current question: Solid Forest Green (#233d32) matching screenshot
            numBtn->setStyleSheet(R"(
                QPushButton {
                    background-color: #233d32;
                    color: #ffffff;
                    border: none;
                    border-radius: 10px;
                    font-size: 14px;
                    font-weight: 700;
                }
            )");
        } else if (m_session->isAnswered(i)) {
            // Answered questions
            numBtn->setStyleSheet(R"(
                QPushButton {
                    background-color: #f0f7f4;
                    color: #233d32;
                    border: 1.5px solid #a3c4b6;
                    border-radius: 10px;
                    font-size: 14px;
                    font-weight: 700;
                }
                QPushButton:hover {
                    background-color: #e0eee7;
                }
            )");
        } else {
            // Unanswered questions
            numBtn->setStyleSheet(R"(
                QPushButton {
                    background-color: #ffffff;
                    color: #374151;
                    border: 1.5px solid #e5e7eb;
                    border-radius: 10px;
                    font-size: 14px;
                    font-weight: 700;
                }
                QPushButton:hover {
                    border-color: #9ca3af;
                    background-color: #f9fafb;
                }
            )");
        }
    }
}

void ExamWindow::onOptionClicked(int optionIndex) {
    m_session->selectAnswer(m_session->getCurrentQuestionIndex(), optionIndex);
    renderCurrentQuestion();
}

void ExamWindow::onPrevClicked() {
    int cur = m_session->getCurrentQuestionIndex();
    if (cur > 0) {
        m_session->setCurrentQuestionIndex(cur - 1);
        renderCurrentQuestion();
    }
}

void ExamWindow::onNextClicked() {
    int cur = m_session->getCurrentQuestionIndex();
    if (cur < m_session->getTotalQuestions() - 1) {
        m_session->setCurrentQuestionIndex(cur + 1);
        renderCurrentQuestion();
    }
}

void ExamWindow::onPageNumberClicked(int questionIndex) {
    m_session->setCurrentQuestionIndex(questionIndex);
    renderCurrentQuestion();
}

void ExamWindow::onSubmitClicked() {
    int answered = m_session->getAnsweredCount();
    int unanswered = m_session->getUnansweredCount();
    int total = m_session->getTotalQuestions();

    QString msg = QString("Are you sure you want to finish and submit your examination?\n\n"
                          "• Answered: %1 of %2\n"
                          "• Unanswered: %3\n\n"
                          "Once submitted, answers cannot be modified.")
                      .arg(answered).arg(total).arg(unanswered);

    auto reply = QMessageBox::question(this, "Confirm Submission", msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        m_timer->stop();
        m_session->setSubmitted(true);
        m_submitted = true;

        StandardEvaluator evaluator;
        m_finalResult = evaluator.evaluate(*m_session);

        QString errorMsg;
        int resId = DatabaseManager::instance().saveResult(m_finalResult, errorMsg);
        m_finalResult.setId(resId);

        auto dbResultOpt = DatabaseManager::instance().getResultById(resId);
        if (dbResultOpt.has_value()) {
            m_finalResult = dbResultOpt.value();
        }

        ResultDialog dlg(m_finalResult, m_session, this);
        dlg.exec();

        accept();
    }
}

void ExamWindow::onTimerTick(int remainingSeconds, const QString& formattedTime) {
    Q_UNUSED(formattedTime);
    m_session->decrementTimer();

    int hours = remainingSeconds / 3600;
    int mins = (remainingSeconds % 3600) / 60;
    int secs = remainingSeconds % 60;

    QString display = QString("%1 : %2 : %3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));

    m_timerValueLabel->setText(display);
}

void ExamWindow::onTimerWarning(int remainingSeconds, const QString& formattedTime) {
    Q_UNUSED(remainingSeconds);
    Q_UNUSED(formattedTime);
    m_timerValueLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #ef4444; letter-spacing: 1px;");
}

void ExamWindow::onTimerTimeout() {
    m_session->setSubmitted(true);
    m_submitted = true;

    QMessageBox::information(this, "Time Expired", "The timer has expired. Your exam will be submitted automatically.");

    StandardEvaluator evaluator;
    m_finalResult = evaluator.evaluate(*m_session);

    QString errorMsg;
    int resId = DatabaseManager::instance().saveResult(m_finalResult, errorMsg);
    m_finalResult.setId(resId);

    auto dbResultOpt = DatabaseManager::instance().getResultById(resId);
    if (dbResultOpt.has_value()) {
        m_finalResult = dbResultOpt.value();
    }

    ResultDialog dlg(m_finalResult, m_session, this);
    dlg.exec();

    accept();
}

void ExamWindow::closeEvent(QCloseEvent* event) {
    if (!m_submitted) {
        auto reply = QMessageBox::question(this, "Exit Exam", 
            "Leaving will submit your current exam. Do you want to submit and exit?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            onSubmitClicked();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}
