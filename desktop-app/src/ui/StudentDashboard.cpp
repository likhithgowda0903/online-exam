#include "ui/StudentDashboard.h"
#include "database/DatabaseManager.h"
#include "ui/ExamWindow.h"
#include "ui/ResultDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>

StudentDashboard::StudentDashboard(StudentPtr student, QWidget* parent)
    : QWidget(parent), m_student(student) {
    setupUI();
    refreshData();
}

void StudentDashboard::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    // Top Header Banner
    auto bannerFrame = new QFrame(this);
    bannerFrame->setObjectName("cardFrame");
    auto bannerLayout = new QHBoxLayout(bannerFrame);
    bannerLayout->setContentsMargins(20, 16, 20, 16);

    auto userDetailsBox = new QVBoxLayout();
    m_welcomeLabel = new QLabel(QString("Welcome back, %1!").arg(m_student->getFullName()), bannerFrame);
    m_welcomeLabel->setObjectName("titleLabel");

    m_studentIdBadge = new QLabel(QString("Student ID: %1  |  Email: %2").arg(m_student->getStudentId()).arg(m_student->getEmail()), bannerFrame);
    m_studentIdBadge->setObjectName("subtitleLabel");

    userDetailsBox->addWidget(m_welcomeLabel);
    userDetailsBox->addWidget(m_studentIdBadge);
    bannerLayout->addLayout(userDetailsBox);

    bannerLayout->addStretch();

    // Refresh & Logout Buttons
    auto actionBtnBox = new QHBoxLayout();
    auto openDbBtn = new QPushButton("🗄️ Open Database", bannerFrame);
    openDbBtn->setObjectName("secondaryButton");
    openDbBtn->setToolTip("Open actual SQLite database in DB Browser for SQLite");
    openDbBtn->setCursor(Qt::PointingHandCursor);
    connect(openDbBtn, &QPushButton::clicked, this, &StudentDashboard::handleOpenDatabase);

    auto refreshBtn = new QPushButton("🔄 Refresh", bannerFrame);
    refreshBtn->setObjectName("secondaryButton");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &StudentDashboard::refreshData);

    auto logoutBtn = new QPushButton("🚪 Sign Out", bannerFrame);
    logoutBtn->setObjectName("dangerButton");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    connect(logoutBtn, &QPushButton::clicked, this, &StudentDashboard::logoutRequested);

    actionBtnBox->addWidget(openDbBtn);
    actionBtnBox->addWidget(refreshBtn);
    actionBtnBox->addWidget(logoutBtn);
    bannerLayout->addLayout(actionBtnBox);

    mainLayout->addWidget(bannerFrame);

    // Dynamic Statistics Cards
    auto statsLayout = new QGridLayout();
    statsLayout->setSpacing(14);

    auto makeStatCard = [](const QString& label, QLabel*& valLabel, const QString& color = "#f8fafc") -> QFrame* {
        auto card = new QFrame();
        card->setObjectName("statCard");
        auto l = new QVBoxLayout(card);
        l->setContentsMargins(14, 12, 14, 12);
        l->setSpacing(4);

        auto lbl = new QLabel(label, card);
        lbl->setStyleSheet("color: #94a3b8; font-size: 11px; font-weight: bold; text-transform: uppercase;");
        valLabel = new QLabel("0", card);
        valLabel->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: 800;").arg(color));

        l->addWidget(lbl);
        l->addWidget(valLabel);
        return card;
    };

    statsLayout->addWidget(makeStatCard("Total Exams Completed", m_examsTakenStat, "#38bdf8"), 0, 0);
    statsLayout->addWidget(makeStatCard("Average Score", m_averageScoreStat, "#6366f1"), 0, 1);
    statsLayout->addWidget(makeStatCard("Passed Examinations", m_passedExamsStat, "#10b981"), 0, 2);

    mainLayout->addLayout(statsLayout);

    // Section 1: Available Examinations
    auto availHeader = new QLabel("📝 Available Examinations", this);
    availHeader->setStyleSheet("font-size: 16px; font-weight: bold; color: #f8fafc; margin-top: 10px;");
    mainLayout->addWidget(availHeader);

    m_availableQuizzesTable = new QTableWidget(this);
    m_availableQuizzesTable->setColumnCount(7);
    m_availableQuizzesTable->setHorizontalHeaderLabels({"ID", "Examination Title", "Category", "Questions", "Total Marks", "Duration", "Action"});
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_availableQuizzesTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_availableQuizzesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_availableQuizzesTable->setAlternatingRowColors(true);
    m_availableQuizzesTable->setMinimumHeight(180);
    mainLayout->addWidget(m_availableQuizzesTable);

    // Section 2: Examination History
    auto histHeader = new QLabel("📜 Examination History & Past Results", this);
    histHeader->setStyleSheet("font-size: 16px; font-weight: bold; color: #f8fafc; margin-top: 10px;");
    mainLayout->addWidget(histHeader);

    m_historyTable = new QTableWidget(this);
    m_historyTable->setColumnCount(7);
    m_historyTable->setHorizontalHeaderLabels({"Attempt Date", "Exam Title", "Marks", "Score %", "Status", "Time Taken", "Action"});
    m_historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_historyTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setAlternatingRowColors(true);
    m_historyTable->setMinimumHeight(180);
    mainLayout->addWidget(m_historyTable);
}

void StudentDashboard::refreshData() {
    auto& db = DatabaseManager::instance();

    // 1. Refresh Stats
    int completedCount = db.getStudentCompletedExamsCount(m_student->getId());
    int passedCount = db.getStudentPassedExamsCount(m_student->getId());
    double avgScore = db.getStudentAverageScore(m_student->getId());

    m_examsTakenStat->setText(QString::number(completedCount));
    m_passedExamsStat->setText(QString::number(passedCount));
    m_averageScoreStat->setText(QString("%1%").arg(avgScore, 0, 'f', 1));

    // 2. Refresh Available Quizzes Table
    m_availableQuizzes = db.getAllQuizzes(true);
    m_availableQuizzesTable->setRowCount(static_cast<int>(m_availableQuizzes.size()));

    for (int i = 0; i < static_cast<int>(m_availableQuizzes.size()); ++i) {
        const auto& q = m_availableQuizzes[i];
        m_availableQuizzesTable->setItem(i, 0, new QTableWidgetItem(QString::number(q.getId())));
        m_availableQuizzesTable->setItem(i, 1, new QTableWidgetItem(q.getTitle()));
        m_availableQuizzesTable->setItem(i, 2, new QTableWidgetItem(q.getCategory()));
        m_availableQuizzesTable->setItem(i, 3, new QTableWidgetItem(QString::number(q.getQuestionCount())));
        m_availableQuizzesTable->setItem(i, 4, new QTableWidgetItem(QString::number(q.getTotalMarks())));
        m_availableQuizzesTable->setItem(i, 5, new QTableWidgetItem(QString("%1 min").arg(q.getDurationMinutes())));

        auto startBtn = new QPushButton("Start Exam ▶", this);
        startBtn->setObjectName("successButton");
        startBtn->setCursor(Qt::PointingHandCursor);
        int quizId = q.getId();
        connect(startBtn, &QPushButton::clicked, [this, quizId]() {
            onStartExamClicked(quizId);
        });

        m_availableQuizzesTable->setCellWidget(i, 6, startBtn);
    }

    // 3. Refresh Examination History Table
    m_studentResults = db.getResultsForStudent(m_student->getId());
    m_historyTable->setRowCount(static_cast<int>(m_studentResults.size()));

    for (int i = 0; i < static_cast<int>(m_studentResults.size()); ++i) {
        const auto& res = m_studentResults[i];
        m_historyTable->setItem(i, 0, new QTableWidgetItem(res.getFormattedDate()));
        m_historyTable->setItem(i, 1, new QTableWidgetItem(res.getQuizTitle()));
        m_historyTable->setItem(i, 2, new QTableWidgetItem(QString("%1 / %2").arg(res.getMarksObtained()).arg(res.getTotalMarks())));
        m_historyTable->setItem(i, 3, new QTableWidgetItem(QString("%1%").arg(res.getPercentage(), 0, 'f', 1)));

        auto statusItem = new QTableWidgetItem(res.getStatusString());
        if (res.isPassed()) {
            statusItem->setForeground(QBrush(QColor("#10b981")));
        } else {
            statusItem->setForeground(QBrush(QColor("#ef4444")));
        }
        statusItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        m_historyTable->setItem(i, 4, statusItem);

        m_historyTable->setItem(i, 5, new QTableWidgetItem(res.getFormattedTimeTaken()));

        auto viewBtn = new QPushButton("Scorecard 📋", this);
        viewBtn->setObjectName("secondaryButton");
        viewBtn->setCursor(Qt::PointingHandCursor);
        int resId = res.getId();
        connect(viewBtn, &QPushButton::clicked, [this, resId]() {
            onViewResultClicked(resId);
        });

        m_historyTable->setCellWidget(i, 6, viewBtn);
    }
}

void StudentDashboard::onStartExamClicked(int quizId) {
    auto quizOpt = DatabaseManager::instance().getQuizById(quizId, true);
    if (!quizOpt.has_value()) {
        QMessageBox::critical(this, "Error", "Selected examination not found.");
        return;
    }

    Quiz quiz = quizOpt.value();
    if (quiz.getQuestionCount() == 0) {
        QMessageBox::warning(this, "Empty Quiz", "This quiz currently does not contain any questions. Please select another quiz.");
        return;
    }

    QString confirmMsg = QString("You are about to start the examination:\n\n"
                                 "Title: %1\n"
                                 "Duration: %2 minutes\n"
                                 "Questions: %3\n"
                                 "Passing Score: %4%\n\n"
                                 "Do you want to proceed now?")
                             .arg(quiz.getTitle())
                             .arg(quiz.getDurationMinutes())
                             .arg(quiz.getQuestionCount())
                             .arg(quiz.getPassPercentage());

    auto reply = QMessageBox::question(this, "Commence Examination", confirmMsg, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        // Dynamically randomize questions and shuffle options for this attempt
        quiz.randomizeQuestions(true);

        auto session = std::make_shared<ExamSession>(m_student->getId(), quiz);
        ExamWindow examWin(session, this);
        examWin.exec();

        // Refresh dashboard statistics and history after exam completion
        refreshData();
    }
}

void StudentDashboard::onViewResultClicked(int resultId) {
    auto resOpt = DatabaseManager::instance().getResultById(resultId);
    if (resOpt.has_value()) {
        ResultDialog dlg(resOpt.value(), nullptr, this);
        dlg.exec();
    } else {
        QMessageBox::critical(this, "Error", "Result record could not be loaded.");
    }
}

void StudentDashboard::handleOpenDatabase() {
    DatabaseManager::instance().openInDbBrowser(this);
}
