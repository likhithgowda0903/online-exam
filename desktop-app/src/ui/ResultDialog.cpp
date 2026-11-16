#include "ui/ResultDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>

ResultDialog::ResultDialog(const Result& result, ExamSessionPtr session, QWidget* parent)
    : QDialog(parent), m_result(result), m_hasSessionReview(session != nullptr), m_session(session) {
    setWindowTitle("Official Examination Score Report");
    setMinimumSize(700, 650);
    resize(760, 720);
    setupUI();
}

void ResultDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    // Header Card
    auto headerFrame = new QFrame(this);
    headerFrame->setObjectName("cardFrame");
    auto headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setAlignment(Qt::AlignCenter);
    headerLayout->setSpacing(8);

    auto title = new QLabel("📋 Official Scorecard", headerFrame);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(title);

    auto quizTitle = new QLabel(m_result.getQuizTitle(), headerFrame);
    quizTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #38bdf8;");
    quizTitle->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(quizTitle);

    // Pass / Fail Badge
    auto statusBadge = new QLabel(headerFrame);
    if (m_result.isPassed()) {
        statusBadge->setText("✔ PASSED");
        statusBadge->setObjectName("badgeSuccess");
    } else {
        statusBadge->setText("✖ FAILED");
        statusBadge->setObjectName("badgeFail");
    }
    statusBadge->setAlignment(Qt::AlignCenter);
    statusBadge->setFixedHeight(32);
    statusBadge->setFixedWidth(140);
    headerLayout->addWidget(statusBadge, 0, Qt::AlignCenter);

    mainLayout->addWidget(headerFrame);

    // Performance Metrics Grid
    auto statsGrid = new QGridLayout();
    statsGrid->setSpacing(12);

    auto createStatCard = [](const QString& label, const QString& value, const QString& color = "#f8fafc") -> QFrame* {
        auto card = new QFrame();
        card->setObjectName("statCard");
        auto l = new QVBoxLayout(card);
        l->setContentsMargins(12, 12, 12, 12);
        l->setSpacing(4);

        auto lbl = new QLabel(label, card);
        lbl->setStyleSheet("color: #94a3b8; font-size: 11px; font-weight: bold; text-transform: uppercase;");
        auto val = new QLabel(value, card);
        val->setStyleSheet(QString("color: %1; font-size: 20px; font-weight: 800;").arg(color));

        l->addWidget(lbl);
        l->addWidget(val);
        return card;
    };

    statsGrid->addWidget(createStatCard("Score Percentage", QString("%1%").arg(m_result.getPercentage(), 0, 'f', 1), m_result.isPassed() ? "#34d399" : "#f87171"), 0, 0);
    statsGrid->addWidget(createStatCard("Marks Obtained", QString("%1 / %2").arg(m_result.getMarksObtained()).arg(m_result.getTotalMarks()), "#38bdf8"), 0, 1);
    statsGrid->addWidget(createStatCard("Total Questions", QString::number(m_result.getTotalQuestions())), 0, 2);

    statsGrid->addWidget(createStatCard("Correct Answers", QString::number(m_result.getCorrectAnswers()), "#10b981"), 1, 0);
    statsGrid->addWidget(createStatCard("Incorrect Answers", QString::number(m_result.getIncorrectAnswers()), "#ef4444"), 1, 1);
    statsGrid->addWidget(createStatCard("Unanswered", QString::number(m_result.getUnansweredQuestions()), "#f59e0b"), 1, 2);

    mainLayout->addLayout(statsGrid);

    // Candidate and Time Details
    auto detailsFrame = new QFrame(this);
    detailsFrame->setObjectName("statCard");
    auto detailsLayout = new QHBoxLayout(detailsFrame);
    
    auto sNameLbl = new QLabel(QString("<b>Candidate:</b> %1").arg(m_result.getStudentFullName().isEmpty() ? m_result.getStudentUsername() : m_result.getStudentFullName()), detailsFrame);
    auto timeLbl = new QLabel(QString("<b>Time Taken:</b> %1").arg(m_result.getFormattedTimeTaken()), detailsFrame);
    auto dateLbl = new QLabel(QString("<b>Date:</b> %1").arg(m_result.getFormattedDate()), detailsFrame);

    detailsLayout->addWidget(sNameLbl);
    detailsLayout->addWidget(timeLbl);
    detailsLayout->addWidget(dateLbl);
    mainLayout->addWidget(detailsFrame);

    // If active session is available, show Question Review Table
    if (m_hasSessionReview && m_session) {
        auto reviewLabel = new QLabel("Detailed Question Breakdown & Explanations", this);
        reviewLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #e2e8f0; margin-top: 10px;");
        mainLayout->addWidget(reviewLabel);

        auto table = new QTableWidget(this);
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"Q#", "Question Prompt", "Your Answer", "Correct Answer", "Result & Explanation"});
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setAlternatingRowColors(true);

        const Quiz& quiz = m_session->getQuiz();
        table->setRowCount(quiz.getQuestionCount());

        for (int i = 0; i < quiz.getQuestionCount(); ++i) {
            const Question* q = quiz.getQuestionAt(i);
            if (!q) continue;

            int selected = m_session->getSelectedAnswer(i);
            int correct = q->getCorrectOptionIndex();

            QString yourAnswerText = selected == -1 ? "Not Answered" : QString("%1. %2").arg(QChar('A' + selected)).arg(q->getOption(selected));
            QString correctAnswerText = QString("%1. %2").arg(QChar('A' + correct)).arg(q->getOption(correct));

            table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            table->setItem(i, 1, new QTableWidgetItem(q->getQuestionText()));
            
            auto yourItem = new QTableWidgetItem(yourAnswerText);
            if (selected == -1) {
                yourItem->setForeground(QBrush(QColor("#f59e0b")));
            } else if (selected == correct) {
                yourItem->setForeground(QBrush(QColor("#10b981")));
            } else {
                yourItem->setForeground(QBrush(QColor("#ef4444")));
            }
            table->setItem(i, 2, yourItem);

            auto correctItem = new QTableWidgetItem(correctAnswerText);
            correctItem->setForeground(QBrush(QColor("#10b981")));
            table->setItem(i, 3, correctItem);

            QString resultExplain = (selected == correct ? "✔ Correct (+ " + QString::number(q->getMarks()) + " marks)\n" : "✖ Incorrect (0 marks)\n");
            if (!q->getExplanation().isEmpty()) {
                resultExplain += "Reason: " + q->getExplanation();
            }
            table->setItem(i, 4, new QTableWidgetItem(resultExplain));
        }

        mainLayout->addWidget(table);
    }

    // Close button
    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto closeBtn = new QPushButton("Close Report", this);
    closeBtn->setMinimumWidth(140);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
}
