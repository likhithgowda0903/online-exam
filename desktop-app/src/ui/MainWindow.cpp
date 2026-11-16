#include "ui/MainWindow.h"
#include "ui/LoginDialog.h"
#include "ui/StudentDashboard.h"
#include "ui/AdminDashboard.h"
#include "database/DatabaseManager.h"
#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_currentUser(nullptr), m_stackedWidget(new QStackedWidget(this)) {
    setWindowTitle("Online Examination System - Professional Edition");
    setMinimumSize(1100, 750);
    resize(1200, 800);
    setupUI();
    showLogin();
}

void MainWindow::setupUI() {
    setCentralWidget(m_stackedWidget);

    auto menuBar = this->menuBar();
    auto* dbMenu = menuBar->addMenu("🗄️ Database");
    auto* openDbAction = dbMenu->addAction("Open in DB Browser for SQLite");
    openDbAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(openDbAction, &QAction::triggered, this, [this]() {
        DatabaseManager::instance().openInDbBrowser(this);
    });

    auto statusBar = this->statusBar();
    statusBar->setStyleSheet("background-color: #0b1120; color: #94a3b8; font-size: 11px; padding: 4px;");
    statusBar->showMessage("System Ready • SQLite Database Connected • C++17 OOP Architecture");
}

void MainWindow::showLogin() {
    LoginDialog loginDlg(this);
    if (loginDlg.exec() == QDialog::Accepted) {
        m_currentUser = loginDlg.getAuthenticatedUser();
        if (m_currentUser) {
            loadUserDashboard();
        } else {
            close();
        }
    } else {
        close();
    }
}

void MainWindow::loadUserDashboard() {
    // Clear any previous dashboard widgets
    while (m_stackedWidget->count() > 0) {
        auto* w = m_stackedWidget->widget(0);
        m_stackedWidget->removeWidget(w);
        delete w;
    }

    if (!m_currentUser) return;

    // Polymorphic dashboard dispatch based on User::getRole()
    if (m_currentUser->getRole() == "Admin") {
        auto admin = std::dynamic_pointer_cast<Admin>(m_currentUser);
        if (admin) {
            setWindowTitle(QString("Online Examination System [Administrator Portal] - Logged in as: %1").arg(admin->getFullName()));
            auto adminDash = new AdminDashboard(admin, this);
            connect(adminDash, &AdminDashboard::logoutRequested, this, &MainWindow::handleLogout);
            m_stackedWidget->addWidget(adminDash);
            m_stackedWidget->setCurrentWidget(adminDash);
            statusBar()->showMessage(QString("Administrator Session Active: %1 (%2)").arg(admin->getFullName()).arg(admin->getAdminCode()));
        }
    } else {
        auto student = std::dynamic_pointer_cast<Student>(m_currentUser);
        if (student) {
            setWindowTitle(QString("Online Examination System [Student Portal] - %1 (%2)").arg(student->getFullName()).arg(student->getStudentId()));
            auto studentDash = new StudentDashboard(student, this);
            connect(studentDash, &StudentDashboard::logoutRequested, this, &MainWindow::handleLogout);
            m_stackedWidget->addWidget(studentDash);
            m_stackedWidget->setCurrentWidget(studentDash);
            statusBar()->showMessage(QString("Student Session Active: %1 (ID: %2)").arg(student->getFullName()).arg(student->getStudentId()));
        }
    }
}

void MainWindow::handleLogout() {
    auto reply = QMessageBox::question(this, "Confirm Sign Out", 
        "Are you sure you want to sign out from the examination portal?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (reply == QMessageBox::Yes) {
        m_currentUser = nullptr;
        statusBar()->showMessage("Logged out. Please sign in.");
        showLogin();
    }
}
