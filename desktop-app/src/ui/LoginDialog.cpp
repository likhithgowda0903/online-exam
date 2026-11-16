#include "ui/LoginDialog.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QGroupBox>
#include <QTabBar>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent), m_authenticatedUser(nullptr) {
    setWindowTitle("Online Examination Portal - Role-Based Access Control");
    setMinimumSize(520, 640);
    resize(540, 680);
    setupUI();
}

void LoginDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 28, 28, 28);
    mainLayout->setSpacing(18);

    // Header Title & Branding
    auto headerLayout = new QVBoxLayout();
    headerLayout->setAlignment(Qt::AlignCenter);
    
    auto appTitle = new QLabel("🎓 Online Examination System", this);
    appTitle->setObjectName("titleLabel");
    appTitle->setAlignment(Qt::AlignCenter);
    
    auto appSubtitle = new QLabel("Secure Role-Based Portal for Students & Institutional Administrators", this);
    appSubtitle->setObjectName("subtitleLabel");
    appSubtitle->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(appTitle);
    headerLayout->addWidget(appSubtitle);
    mainLayout->addLayout(headerLayout);

    // Role-Based Tab Navigation: Student and Administrator Login
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createStudentLoginTab(), "🎓 Student Login");
    m_tabWidget->addTab(createAdminLoginTab(), "🛡️ Administrator Login");
    m_tabWidget->addTab(createRegisterTab(), "");
    auto tabBar = m_tabWidget->findChild<QTabBar*>();
    if (tabBar) {
        tabBar->setTabVisible(2, false);
    }

    mainLayout->addWidget(m_tabWidget);

    // Database access button on login screen
    auto footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(5, 6, 5, 0);
    auto dbBtn = new QPushButton("🗄️ Open Database (SQLite)", this);
    dbBtn->setObjectName("secondaryButton");
    dbBtn->setToolTip("Open actual SQLite database in DB Browser for SQLite");
    dbBtn->setCursor(Qt::PointingHandCursor);
    connect(dbBtn, &QPushButton::clicked, this, [this]() {
        DatabaseManager::instance().openInDbBrowser(this);
    });
    footerLayout->addWidget(dbBtn);
    footerLayout->addStretch();
    mainLayout->addLayout(footerLayout);
}

QWidget* LoginDialog::createStudentLoginTab() {
    auto tab = new QWidget(this);
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(22, 22, 22, 22);
    layout->setSpacing(16);

    // Banner card
    auto banner = new QFrame(tab);
    banner->setStyleSheet("background-color: rgba(37, 99, 235, 0.12); border: 1px solid rgba(59, 130, 246, 0.3); border-radius: 8px; padding: 10px;");
    auto bannerLayout = new QVBoxLayout(banner);
    bannerLayout->setContentsMargins(8, 6, 8, 6);
    bannerLayout->setSpacing(3);

    auto bannerTitle = new QLabel("👨‍🎓 Candidate Examination Portal", banner);
    bannerTitle->setStyleSheet("font-weight: 700; color: #93c5fd; font-size: 13px;");
    auto bannerSub = new QLabel("Sign in with your student credentials or Roll ID to take timed online examinations.", banner);
    bannerSub->setStyleSheet("color: #cbd5e1; font-size: 11px;");
    bannerSub->setWordWrap(true);
    bannerLayout->addWidget(bannerTitle);
    bannerLayout->addWidget(bannerSub);
    layout->addWidget(banner);

    // Form
    auto formLayout = new QFormLayout();
    formLayout->setSpacing(14);

    m_studentUsernameEdit = new QLineEdit(tab);
    m_studentUsernameEdit->setPlaceholderText("Enter student username or ID");
    formLayout->addRow("Student Username:", m_studentUsernameEdit);

    m_studentPasswordEdit = new QLineEdit(tab);
    m_studentPasswordEdit->setPlaceholderText("Enter student password");
    m_studentPasswordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Password:", m_studentPasswordEdit);

    layout->addLayout(formLayout);

    // Error label
    m_studentErrorLabel = new QLabel(tab);
    m_studentErrorLabel->setStyleSheet("color: #ef4444; font-weight: bold; font-size: 12px;");
    m_studentErrorLabel->setWordWrap(true);
    m_studentErrorLabel->hide();
    layout->addWidget(m_studentErrorLabel);

    // Login Button
    m_studentLoginButton = new QPushButton("Sign In as Student ▶", tab);
    m_studentLoginButton->setCursor(Qt::PointingHandCursor);
    m_studentLoginButton->setMinimumHeight(44);
    connect(m_studentLoginButton, &QPushButton::clicked, this, &LoginDialog::handleStudentLogin);
    connect(m_studentPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::handleStudentLogin);
    layout->addWidget(m_studentLoginButton);

    // Register / Signup Prompt below Student Login
    auto signupLayout = new QHBoxLayout();
    signupLayout->setAlignment(Qt::AlignCenter);
    auto signupPrompt = new QLabel("Don't have an account? <a href='#signup' style='color: #60a5fa; text-decoration: none; font-weight: bold;'>Signup</a>", tab);
    signupPrompt->setStyleSheet("color: #94a3b8; font-size: 13px; margin-top: 8px;");
    connect(signupPrompt, &QLabel::linkActivated, [this]() {
        switchToRegister(false);
    });
    signupLayout->addWidget(signupPrompt);
    layout->addLayout(signupLayout);

    layout->addStretch();
    return tab;
}

QWidget* LoginDialog::createAdminLoginTab() {
    auto tab = new QWidget(this);
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(22, 22, 22, 22);
    layout->setSpacing(16);

    // Admin security badge banner
    auto banner = new QFrame(tab);
    banner->setStyleSheet("background-color: rgba(139, 92, 246, 0.12); border: 1px solid rgba(139, 92, 246, 0.35); border-radius: 8px; padding: 10px;");
    auto bannerLayout = new QVBoxLayout(banner);
    bannerLayout->setContentsMargins(8, 6, 8, 6);
    bannerLayout->setSpacing(3);

    auto bannerTitle = new QLabel("🛡️ Administrator Control Center", banner);
    bannerTitle->setStyleSheet("font-weight: 700; color: #c4b5fd; font-size: 13px;");
    auto bannerSub = new QLabel("Restricted administrative console for Faculty, Examination Officers, and System Administrators.", banner);
    bannerSub->setStyleSheet("color: #cbd5e1; font-size: 11px;");
    bannerSub->setWordWrap(true);
    bannerLayout->addWidget(bannerTitle);
    bannerLayout->addWidget(bannerSub);
    layout->addWidget(banner);

    // Form
    auto formLayout = new QFormLayout();
    formLayout->setSpacing(14);

    m_adminUsernameEdit = new QLineEdit(tab);
    m_adminUsernameEdit->setPlaceholderText("Enter administrator username");
    formLayout->addRow("Admin Username:", m_adminUsernameEdit);

    m_adminPasswordEdit = new QLineEdit(tab);
    m_adminPasswordEdit->setPlaceholderText("Enter administrator password");
    m_adminPasswordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Security Password:", m_adminPasswordEdit);

    layout->addLayout(formLayout);

    // Error label
    m_adminErrorLabel = new QLabel(tab);
    m_adminErrorLabel->setStyleSheet("color: #ef4444; font-weight: bold; font-size: 12px;");
    m_adminErrorLabel->setWordWrap(true);
    m_adminErrorLabel->hide();
    layout->addWidget(m_adminErrorLabel);

    // Admin Login Button
    m_adminLoginButton = new QPushButton("Sign In as Administrator 🛡️", tab);
    m_adminLoginButton->setObjectName("adminButton");
    m_adminLoginButton->setStyleSheet("background-color: #7c3aed; color: #ffffff; font-weight: 700; font-size: 13px;");
    m_adminLoginButton->setCursor(Qt::PointingHandCursor);
    m_adminLoginButton->setMinimumHeight(44);
    connect(m_adminLoginButton, &QPushButton::clicked, this, &LoginDialog::handleAdminLogin);
    connect(m_adminPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::handleAdminLogin);
    connect(m_adminUsernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::handleAdminLogin);
    layout->addWidget(m_adminLoginButton);

    // Register / Signup Prompt below Admin Login
    auto adminSignupLayout = new QHBoxLayout();
    adminSignupLayout->setAlignment(Qt::AlignCenter);
    auto adminSignupPrompt = new QLabel("Don't have an account? <a href='#signup' style='color: #c4b5fd; text-decoration: none; font-weight: bold;'>Signup</a>", tab);
    adminSignupPrompt->setStyleSheet("color: #94a3b8; font-size: 13px; margin-top: 8px;");
    connect(adminSignupPrompt, &QLabel::linkActivated, [this]() {
        switchToRegister(true);
    });
    adminSignupLayout->addWidget(adminSignupPrompt);
    layout->addLayout(adminSignupLayout);

    layout->addStretch();
    return tab;
}

QWidget* LoginDialog::createRegisterTab() {
    auto tab = new QWidget(this);
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    // Registration Banner
    auto banner = new QFrame(tab);
    banner->setStyleSheet("background-color: rgba(37, 99, 235, 0.12); border: 1px solid rgba(59, 130, 246, 0.3); border-radius: 8px; padding: 10px;");
    auto bannerLayout = new QVBoxLayout(banner);
    bannerLayout->setContentsMargins(8, 6, 8, 6);
    bannerLayout->setSpacing(3);

    m_regBannerTitle = new QLabel("📝 Account Registration", banner);
    m_regBannerTitle->setStyleSheet("font-weight: 700; color: #93c5fd; font-size: 13px;");
    m_regBannerSub = new QLabel("Enroll as a student candidate or register an administrator account.", banner);
    m_regBannerSub->setStyleSheet("color: #cbd5e1; font-size: 11px;");
    m_regBannerSub->setWordWrap(true);
    bannerLayout->addWidget(m_regBannerTitle);
    bannerLayout->addWidget(m_regBannerSub);
    layout->addWidget(banner);

    auto formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    // Role Selection
    m_regRoleCombo = new QComboBox(tab);
    m_regRoleCombo->addItem("🎓 Student Candidate", "Student");
    m_regRoleCombo->addItem("🛡️ Administrator", "Admin");
    m_regRoleCombo->setMinimumHeight(36);
    connect(m_regRoleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int idx) {
        bool isAdmin = (idx == 1);
        if (isAdmin) {
            m_regBannerTitle->setText("🛡️ Administrator Registration");
            m_regBannerTitle->setStyleSheet("font-weight: 700; color: #c4b5fd; font-size: 13px;");
            m_regBannerSub->setText("Create an administrative supervisor account to oversee exams and question banks.");
            m_regIdLabel->setText("Admin Staff ID *:");
            m_regStudentIdEdit->setPlaceholderText("e.g. ADM002 or ADM_STAFF");
            m_registerButton->setText("Create Administrator Account 🛡️");
            m_registerButton->setStyleSheet("background-color: #7c3aed; color: #ffffff; font-weight: 700; font-size: 13px;");
        } else {
            m_regBannerTitle->setText("📝 Student Registration");
            m_regBannerTitle->setStyleSheet("font-weight: 700; color: #93c5fd; font-size: 13px;");
            m_regBannerSub->setText("Enroll as a student candidate for timed online course examinations.");
            m_regIdLabel->setText("Student ID *:");
            m_regStudentIdEdit->setPlaceholderText("e.g. STU1002");
            m_registerButton->setText("Create Student Account ▶");
            m_registerButton->setStyleSheet("");
        }
    });
    formLayout->addRow("Account Role *:", m_regRoleCombo);

    m_regFullNameEdit = new QLineEdit(tab);
    m_regFullNameEdit->setPlaceholderText("e.g. John Doe");
    formLayout->addRow("Full Name *:", m_regFullNameEdit);

    m_regIdLabel = new QLabel("Student ID *:", tab);
    m_regStudentIdEdit = new QLineEdit(tab);
    m_regStudentIdEdit->setPlaceholderText("e.g. STU1002");
    formLayout->addRow(m_regIdLabel, m_regStudentIdEdit);

    m_regEmailEdit = new QLineEdit(tab);
    m_regEmailEdit->setPlaceholderText("e.g. john@student.edu");
    formLayout->addRow("Email Address *:", m_regEmailEdit);

    m_regUsernameEdit = new QLineEdit(tab);
    m_regUsernameEdit->setPlaceholderText("Choose a unique username");
    formLayout->addRow("Username *:", m_regUsernameEdit);

    m_regPasswordEdit = new QLineEdit(tab);
    m_regPasswordEdit->setPlaceholderText("Create a password (min 6 chars)");
    m_regPasswordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Password *:", m_regPasswordEdit);

    m_regConfirmPasswordEdit = new QLineEdit(tab);
    m_regConfirmPasswordEdit->setPlaceholderText("Re-type password");
    m_regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Confirm Password *:", m_regConfirmPasswordEdit);

    layout->addLayout(formLayout);

    // Error message label
    m_regErrorLabel = new QLabel(tab);
    m_regErrorLabel->setStyleSheet("color: #ef4444; font-weight: bold; font-size: 12px;");
    m_regErrorLabel->setWordWrap(true);
    m_regErrorLabel->hide();
    layout->addWidget(m_regErrorLabel);

    // Register button
    m_registerButton = new QPushButton("Create Student Account ▶", tab);
    m_registerButton->setObjectName("successButton");
    m_registerButton->setCursor(Qt::PointingHandCursor);
    m_registerButton->setMinimumHeight(44);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginDialog::handleRegister);
    layout->addWidget(m_registerButton);

    // Already have an account prompt (as shown in UI reference)
    auto loginPromptLayout = new QHBoxLayout();
    loginPromptLayout->setAlignment(Qt::AlignCenter);
    auto loginPrompt = new QLabel("Already have an account? <a href='#login' style='color: #60a5fa; text-decoration: none; font-weight: bold;'>Sign in here</a>", tab);
    loginPrompt->setStyleSheet("color: #94a3b8; font-size: 13px; margin-top: 8px;");
    connect(loginPrompt, &QLabel::linkActivated, [this]() {
        bool isAdmin = (m_regRoleCombo && m_regRoleCombo->currentIndex() == 1);
        switchToLogin(isAdmin);
    });
    loginPromptLayout->addWidget(loginPrompt);
    layout->addLayout(loginPromptLayout);

    layout->addStretch();
    return tab;
}

void LoginDialog::handleStudentLogin() {
    QString username = m_studentUsernameEdit->text().trimmed();
    QString password = m_studentPasswordEdit->text();

    m_studentErrorLabel->hide();

    if (username.isEmpty() || password.isEmpty()) {
        m_studentErrorLabel->setText("Please enter both username and password.");
        m_studentErrorLabel->show();
        return;
    }

    QString errorMsg;
    UserPtr user = DatabaseManager::instance().authenticateUser(username, password, errorMsg);

    if (!user) {
        m_studentErrorLabel->setText(errorMsg);
        m_studentErrorLabel->show();
        return;
    }

    m_authenticatedUser = user;
    accept(); // Close dialog with Accepted result
}

void LoginDialog::handleAdminLogin() {
    QString username = m_adminUsernameEdit->text().trimmed();
    QString password = m_adminPasswordEdit->text();

    m_adminErrorLabel->hide();

    if (username.isEmpty() || password.isEmpty()) {
        m_adminErrorLabel->setText("Please enter both administrator username and password.");
        m_adminErrorLabel->show();
        return;
    }

    QString errorMsg;
    UserPtr user = DatabaseManager::instance().authenticateUser(username, password, errorMsg);

    if (!user) {
        m_adminErrorLabel->setText(errorMsg);
        m_adminErrorLabel->show();
        return;
    }

    // Role-verification: Strict enforcement of administrative privileges
    if (user->getRole() != "Admin") {
        m_adminErrorLabel->setText(QString("⛔ Access Denied: The account '%1' is a student account and lacks administrative privileges. Please use the Student Login tab.").arg(username));
        m_adminErrorLabel->show();
        return;
    }

    m_authenticatedUser = user;
    accept(); // Close dialog with Accepted result
}

void LoginDialog::switchToRegister(bool isAdmin) {
    if (m_regRoleCombo) {
        m_regRoleCombo->setCurrentIndex(isAdmin ? 1 : 0);
    }
    m_tabWidget->setCurrentIndex(2); // Registration tab
    m_regFullNameEdit->setFocus();
}

void LoginDialog::switchToLogin(bool isAdmin) {
    m_tabWidget->setCurrentIndex(isAdmin ? 1 : 0);
    if (isAdmin) {
        m_adminUsernameEdit->setFocus();
    } else {
        m_studentUsernameEdit->setFocus();
    }
}

void LoginDialog::handleRegister() {
    m_regErrorLabel->hide();

    bool isAdmin = (m_regRoleCombo && m_regRoleCombo->currentIndex() == 1);
    QString fullName = m_regFullNameEdit->text().trimmed();
    QString studentId = m_regStudentIdEdit->text().trimmed();
    QString email = m_regEmailEdit->text().trimmed();
    QString username = m_regUsernameEdit->text().trimmed();
    QString password = m_regPasswordEdit->text();
    QString confirmPassword = m_regConfirmPasswordEdit->text();

    if (fullName.isEmpty() || studentId.isEmpty() || email.isEmpty() || username.isEmpty() || password.isEmpty()) {
        m_regErrorLabel->setText("All fields marked with * are required.");
        m_regErrorLabel->show();
        return;
    }

    // Email validation regex
    static QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
    if (!emailRegex.match(email).hasMatch()) {
        m_regErrorLabel->setText("Please enter a valid email address (e.g., user@domain.com).");
        m_regErrorLabel->show();
        return;
    }

    if (password.length() < 6) {
        m_regErrorLabel->setText("Password must be at least 6 characters long.");
        m_regErrorLabel->show();
        return;
    }

    if (password != confirmPassword) {
        m_regErrorLabel->setText("Passwords do not match. Please re-enter.");
        m_regErrorLabel->show();
        return;
    }

    QString dbError;
    bool success = false;
    if (isAdmin) {
        success = DatabaseManager::instance().registerAdmin(username, password, fullName, email, studentId, dbError);
    } else {
        success = DatabaseManager::instance().registerStudent(username, password, fullName, email, studentId, dbError);
    }

    if (!success) {
        m_regErrorLabel->setText(dbError);
        m_regErrorLabel->show();
        return;
    }

    QMessageBox::information(this, "Registration Successful", 
        isAdmin ? "Administrator Account created successfully! You can now log in with your administrative credentials."
                : "Student Account created successfully! You can now log in with your candidate credentials.");

    // Clear registration fields
    m_regFullNameEdit->clear();
    m_regStudentIdEdit->clear();
    m_regEmailEdit->clear();
    m_regUsernameEdit->clear();
    m_regPasswordEdit->clear();
    m_regConfirmPasswordEdit->clear();

    if (isAdmin) {
        m_adminUsernameEdit->setText(username);
        m_adminPasswordEdit->clear();
        m_tabWidget->setCurrentIndex(1); // Admin tab
        m_adminPasswordEdit->setFocus();
    } else {
        m_studentUsernameEdit->setText(username);
        m_studentPasswordEdit->clear();
        m_tabWidget->setCurrentIndex(0); // Student tab
        m_studentPasswordEdit->setFocus();
    }
}
