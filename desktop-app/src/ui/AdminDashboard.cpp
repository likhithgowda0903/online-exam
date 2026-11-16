#include "ui/AdminDashboard.h"
#include "database/DatabaseManager.h"
#include "ui/QuizEditorDialog.h"
#include "ui/ResultDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>

AdminDashboard::AdminDashboard(AdminPtr admin, QWidget* parent)
    : QWidget(parent), m_admin(admin) {
    setupUI();
    refreshData();
}

void AdminDashboard::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    // Top Header Banner
    auto bannerFrame = new QFrame(this);
    bannerFrame->setObjectName("cardFrame");
    auto bannerLayout = new QHBoxLayout(bannerFrame);
    bannerLayout->setContentsMargins(20, 16, 20, 16);

    auto adminInfoBox = new QVBoxLayout();
    auto titleLabel = new QLabel(QString("Administrator Control Center - %1").arg(m_admin->getFullName()), bannerFrame);
    titleLabel->setObjectName("titleLabel");

    auto deptLabel = new QLabel(QString("Staff Code: %1  |  Department: %2  |  Role: Administrator")
        .arg(m_admin->getAdminCode()).arg(m_admin->getDepartment()), bannerFrame);
    deptLabel->setObjectName("subtitleLabel");

    adminInfoBox->addWidget(titleLabel);
    adminInfoBox->addWidget(deptLabel);
    bannerLayout->addLayout(adminInfoBox);

    bannerLayout->addStretch();

    // Top Action Buttons
    auto actionBtnBox = new QHBoxLayout();
    auto openDbBtn = new QPushButton("🗄️ Open Database", bannerFrame);
    openDbBtn->setObjectName("secondaryButton");
    openDbBtn->setToolTip("Open actual SQLite database (online_exam.db) in DB Browser for SQLite");
    openDbBtn->setCursor(Qt::PointingHandCursor);
    connect(openDbBtn, &QPushButton::clicked, this, &AdminDashboard::handleOpenDatabase);

    auto refreshBtn = new QPushButton("🔄 Refresh Data", bannerFrame);
    refreshBtn->setObjectName("secondaryButton");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &AdminDashboard::refreshData);

    auto logoutBtn = new QPushButton("🚪 Sign Out", bannerFrame);
    logoutBtn->setObjectName("dangerButton");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    connect(logoutBtn, &QPushButton::clicked, this, &AdminDashboard::logoutRequested);

    actionBtnBox->addWidget(openDbBtn);
    actionBtnBox->addWidget(refreshBtn);
    actionBtnBox->addWidget(logoutBtn);
    bannerLayout->addLayout(actionBtnBox);

    mainLayout->addWidget(bannerFrame);

    // Dynamic Statistics Cards
    auto statsGrid = new QGridLayout();
    statsGrid->setSpacing(14);

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

    statsGrid->addWidget(makeStatCard("Registered Students", m_totalStudentsStat, "#38bdf8"), 0, 0);
    statsGrid->addWidget(makeStatCard("Available Examinations", m_totalQuizzesStat, "#6366f1"), 0, 1);
    statsGrid->addWidget(makeStatCard("Exams Completed", m_completedExamsStat, "#10b981"), 0, 2);
    statsGrid->addWidget(makeStatCard("Global Average Score", m_globalAverageStat, "#f59e0b"), 0, 3);

    mainLayout->addLayout(statsGrid);

    // Management Tabs
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createQuizzesTab(), "📝 Manage Examinations");
    m_tabWidget->addTab(createStudentsTab(), "👥 Manage Students");
    m_tabWidget->addTab(createResultsTab(), "📊 Examination Results & Analytics");
    mainLayout->addWidget(m_tabWidget);
}

QWidget* AdminDashboard::createQuizzesTab() {
    auto tab = new QWidget(this);
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(12);

    auto topBar = new QHBoxLayout();
    auto secTitle = new QLabel("Examination Packages & Question Pools", tab);
    secTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #f8fafc;");
    topBar->addWidget(secTitle);
    topBar->addStretch();

    auto createBtn = new QPushButton("➕ Create New Examination", tab);
    createBtn->setObjectName("successButton");
    createBtn->setCursor(Qt::PointingHandCursor);
    connect(createBtn, &QPushButton::clicked, this, &AdminDashboard::handleCreateQuiz);

    auto editBtn = new QPushButton("✏ Edit Selected", tab);
    editBtn->setObjectName("secondaryButton");
    editBtn->setCursor(Qt::PointingHandCursor);
    connect(editBtn, &QPushButton::clicked, this, &AdminDashboard::handleEditQuiz);

    auto deleteBtn = new QPushButton("🗑 Delete Selected", tab);
    deleteBtn->setObjectName("dangerButton");
    deleteBtn->setCursor(Qt::PointingHandCursor);
    connect(deleteBtn, &QPushButton::clicked, this, &AdminDashboard::handleDeleteQuiz);

    topBar->addWidget(createBtn);
    topBar->addWidget(editBtn);
    topBar->addWidget(deleteBtn);
    layout->addLayout(topBar);

    m_quizzesTable = new QTableWidget(tab);
    m_quizzesTable->setColumnCount(7);
    m_quizzesTable->setHorizontalHeaderLabels({"ID", "Examination Title", "Category", "Questions", "Total Marks", "Duration", "Pass %"});
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_quizzesTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_quizzesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_quizzesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_quizzesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_quizzesTable->setAlternatingRowColors(true);
    layout->addWidget(m_quizzesTable);

    return tab;
}

QWidget* AdminDashboard::createStudentsTab() {
    auto tab = new QWidget(this);
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(12);

    auto topBar = new QHBoxLayout();
    auto secTitle = new QLabel("Enrolled Students Directory", tab);
    secTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #f8fafc;");
    topBar->addWidget(secTitle);
    topBar->addStretch();

    auto delStudentBtn = new QPushButton("🗑 Remove Student Account", tab);
    delStudentBtn->setObjectName("dangerButton");
    delStudentBtn->setCursor(Qt::PointingHandCursor);
    connect(delStudentBtn, &QPushButton::clicked, this, &AdminDashboard::handleDeleteStudent);
    topBar->addWidget(delStudentBtn);
    layout->addLayout(topBar);

    m_studentsTable = new QTableWidget(tab);
    m_studentsTable->setColumnCount(7);
    m_studentsTable->setHorizontalHeaderLabels({"Student ID", "Full Name", "Username", "Email", "Exams Completed", "Average Score", "Registration Date"});
    m_studentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_studentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_studentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentsTable->setAlternatingRowColors(true);
    layout->addWidget(m_studentsTable);

    return tab;
}

QWidget* AdminDashboard::createResultsTab() {
    auto tab = new QWidget(this);
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(12);

    auto topBar = new QHBoxLayout();
    auto secTitle = new QLabel("Institutional Exam Attempts & Reports", tab);
    secTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #f8fafc;");
    topBar->addWidget(secTitle);
    topBar->addStretch();

    auto viewScorecardBtn = new QPushButton("📋 View Detailed Scorecard", tab);
    viewScorecardBtn->setObjectName("secondaryButton");
    viewScorecardBtn->setCursor(Qt::PointingHandCursor);
    connect(viewScorecardBtn, &QPushButton::clicked, this, &AdminDashboard::handleViewResult);
    topBar->addWidget(viewScorecardBtn);
    layout->addLayout(topBar);

    m_resultsTable = new QTableWidget(tab);
    m_resultsTable->setColumnCount(8);
    m_resultsTable->setHorizontalHeaderLabels({"Attempt Date", "Candidate Name", "Username", "Exam Title", "Score %", "Marks", "Status", "Time"});
    m_resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setAlternatingRowColors(true);
    layout->addWidget(m_resultsTable);

    return tab;
}

void AdminDashboard::refreshData() {
    auto& db = DatabaseManager::instance();
    QString adminCode = m_admin ? m_admin->getAdminCode() : "";

    // 1. Refresh Top Statistics (Isolated to this administrator's examinations)
    int studentsCount = db.getTotalStudentsCount();
    int quizzesCount = !adminCode.isEmpty() ? db.getTotalQuizzesCountByAdmin(adminCode) : db.getTotalQuizzesCount();
    int examsCount = db.getTotalExamsCompletedCount();
    double globalAvg = db.getGlobalAverageScore();

    m_totalStudentsStat->setText(QString::number(studentsCount));
    m_totalQuizzesStat->setText(QString::number(quizzesCount));
    m_completedExamsStat->setText(QString::number(examsCount));
    m_globalAverageStat->setText(QString("%1%").arg(globalAvg, 0, 'f', 1));

    // 2. Refresh Quizzes Table (Isolated to this administrator's examinations)
    m_quizzesList = !adminCode.isEmpty() ? db.getQuizzesByAdmin(adminCode, true) : db.getAllQuizzes(true);
    m_quizzesTable->setRowCount(static_cast<int>(m_quizzesList.size()));

    for (int i = 0; i < static_cast<int>(m_quizzesList.size()); ++i) {
        const auto& q = m_quizzesList[i];
        m_quizzesTable->setItem(i, 0, new QTableWidgetItem(QString::number(q.getId())));
        m_quizzesTable->setItem(i, 1, new QTableWidgetItem(q.getTitle()));
        m_quizzesTable->setItem(i, 2, new QTableWidgetItem(q.getCategory()));
        m_quizzesTable->setItem(i, 3, new QTableWidgetItem(QString::number(q.getQuestionCount())));
        m_quizzesTable->setItem(i, 4, new QTableWidgetItem(QString::number(q.getTotalMarks())));
        m_quizzesTable->setItem(i, 5, new QTableWidgetItem(QString("%1 min").arg(q.getDurationMinutes())));
        m_quizzesTable->setItem(i, 6, new QTableWidgetItem(QString("%1%").arg(q.getPassPercentage(), 0, 'f', 1)));
    }

    // 3. Refresh Students Table
    m_studentsList = db.getAllStudents();
    m_studentsTable->setRowCount(static_cast<int>(m_studentsList.size()));

    for (int i = 0; i < static_cast<int>(m_studentsList.size()); ++i) {
        const auto& s = m_studentsList[i];
        m_studentsTable->setItem(i, 0, new QTableWidgetItem(s.getStudentId()));
        m_studentsTable->setItem(i, 1, new QTableWidgetItem(s.getFullName()));
        m_studentsTable->setItem(i, 2, new QTableWidgetItem(s.getUsername()));
        m_studentsTable->setItem(i, 3, new QTableWidgetItem(s.getEmail()));
        m_studentsTable->setItem(i, 4, new QTableWidgetItem(QString::number(s.getTotalExamsTaken())));
        m_studentsTable->setItem(i, 5, new QTableWidgetItem(QString("%1%").arg(s.getAverageScore(), 0, 'f', 1)));
        m_studentsTable->setItem(i, 6, new QTableWidgetItem(s.getCreatedAt().toString("yyyy-MM-dd")));
    }

    // 4. Refresh Results Table
    m_resultsList = db.getAllResults();
    m_resultsTable->setRowCount(static_cast<int>(m_resultsList.size()));

    for (int i = 0; i < static_cast<int>(m_resultsList.size()); ++i) {
        const auto& r = m_resultsList[i];
        m_resultsTable->setItem(i, 0, new QTableWidgetItem(r.getFormattedDate()));
        m_resultsTable->setItem(i, 1, new QTableWidgetItem(r.getStudentFullName()));
        m_resultsTable->setItem(i, 2, new QTableWidgetItem(r.getStudentUsername()));
        m_resultsTable->setItem(i, 3, new QTableWidgetItem(r.getQuizTitle()));
        m_resultsTable->setItem(i, 4, new QTableWidgetItem(QString("%1%").arg(r.getPercentage(), 0, 'f', 1)));
        m_resultsTable->setItem(i, 5, new QTableWidgetItem(QString("%1 / %2").arg(r.getMarksObtained()).arg(r.getTotalMarks())));

        auto statusItem = new QTableWidgetItem(r.getStatusString());
        if (r.isPassed()) {
            statusItem->setForeground(QBrush(QColor("#10b981")));
        } else {
            statusItem->setForeground(QBrush(QColor("#ef4444")));
        }
        statusItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        m_resultsTable->setItem(i, 6, statusItem);

        m_resultsTable->setItem(i, 7, new QTableWidgetItem(r.getFormattedTimeTaken()));
    }
}

void AdminDashboard::handleCreateQuiz() {
    QuizEditorDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Quiz newQuiz = dlg.getQuiz();
        if (m_admin) {
            newQuiz.setCreatedBy(m_admin->getAdminCode());
        }
        QString errorMsg;
        int quizId = DatabaseManager::instance().saveQuiz(newQuiz, errorMsg);
        if (quizId > 0) {
            QMessageBox::information(this, "Success", "Examination created successfully!");
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to save examination: " + errorMsg);
        }
    }
}

void AdminDashboard::handleEditQuiz() {
    int row = m_quizzesTable->currentRow();
    if (row < 0 || row >= static_cast<int>(m_quizzesList.size())) {
        QMessageBox::warning(this, "Select Examination", "Please select an examination to edit.");
        return;
    }

    Quiz targetQuiz = m_quizzesList[row];
    QuizEditorDialog dlg(targetQuiz, this);
    if (dlg.exec() == QDialog::Accepted) {
        Quiz updatedQuiz = dlg.getQuiz();
        QString errorMsg;
        if (DatabaseManager::instance().updateQuiz(updatedQuiz, errorMsg)) {
            // Delete and re-insert questions to update questions list cleanly
            auto existingQuestions = DatabaseManager::instance().getQuestionsForQuiz(updatedQuiz.getId());
            for (const auto& eq : existingQuestions) {
                DatabaseManager::instance().deleteQuestion(eq.getId());
            }
            for (const auto& nq : updatedQuiz.getQuestions()) {
                Question qCopy = nq;
                qCopy.setQuizId(updatedQuiz.getId());
                DatabaseManager::instance().saveQuestion(qCopy, errorMsg);
            }

            QMessageBox::information(this, "Success", "Examination updated successfully!");
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to update examination: " + errorMsg);
        }
    }
}

void AdminDashboard::handleDeleteQuiz() {
    int row = m_quizzesTable->currentRow();
    if (row < 0 || row >= static_cast<int>(m_quizzesList.size())) {
        QMessageBox::warning(this, "Select Examination", "Please select an examination to delete.");
        return;
    }

    const Quiz& q = m_quizzesList[row];
    auto reply = QMessageBox::question(this, "Confirm Deletion", 
        QString("Are you sure you want to delete '%1'? All corresponding student results will also be deleted.").arg(q.getTitle()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteQuiz(q.getId())) {
            QMessageBox::information(this, "Deleted", "Examination deleted successfully.");
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete examination.");
        }
    }
}

void AdminDashboard::handleDeleteStudent() {
    int row = m_studentsTable->currentRow();
    if (row < 0 || row >= static_cast<int>(m_studentsList.size())) {
        QMessageBox::warning(this, "Select Student", "Please select a student from the directory to remove.");
        return;
    }

    const Student& s = m_studentsList[row];
    auto reply = QMessageBox::question(this, "Confirm Removal",
        QString("Are you sure you want to remove student account '%1' (%2)? All associated results will be deleted.").arg(s.getFullName()).arg(s.getStudentId()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteStudent(s.getId())) {
            QMessageBox::information(this, "Removed", "Student account removed successfully.");
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to remove student account.");
        }
    }
}

void AdminDashboard::handleViewResult() {
    int row = m_resultsTable->currentRow();
    if (row < 0 || row >= static_cast<int>(m_resultsList.size())) {
        QMessageBox::warning(this, "Select Record", "Please select an examination result row to inspect.");
        return;
    }

    const Result& r = m_resultsList[row];
    ResultDialog dlg(r, nullptr, this);
    dlg.exec();
}

void AdminDashboard::handleOpenDatabase() {
    DatabaseManager::instance().openInDbBrowser(this);
}
