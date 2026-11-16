#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include "models/User.h"

/**
 * @file LoginDialog.h
 * @brief Modal dialog providing dedicated role-based logins (Student & Administrator) and user registration.
 */
class LoginDialog : public QDialog {
    Q_OBJECT

private:
    QTabWidget* m_tabWidget;

    // Student Login Form Widgets
    QLineEdit* m_studentUsernameEdit;
    QLineEdit* m_studentPasswordEdit;
    QLabel* m_studentErrorLabel;
    QPushButton* m_studentLoginButton;

    // Administrator Login Form Widgets
    QLineEdit* m_adminUsernameEdit;
    QLineEdit* m_adminPasswordEdit;
    QLabel* m_adminErrorLabel;
    QPushButton* m_adminLoginButton;

    // Register Form Widgets
    QComboBox* m_regRoleCombo;
    QLabel* m_regBannerTitle;
    QLabel* m_regBannerSub;
    QLineEdit* m_regFullNameEdit;
    QLabel* m_regIdLabel;
    QLineEdit* m_regStudentIdEdit;
    QLineEdit* m_regEmailEdit;
    QLineEdit* m_regUsernameEdit;
    QLineEdit* m_regPasswordEdit;
    QLineEdit* m_regConfirmPasswordEdit;
    QLabel* m_regErrorLabel;
    QPushButton* m_registerButton;

    // Navigation helpers
    void switchToRegister(bool isAdmin);
    void switchToLogin(bool isAdmin);

    // Authenticated user result
    UserPtr m_authenticatedUser;

    void setupUI();
    QWidget* createStudentLoginTab();
    QWidget* createAdminLoginTab();
    QWidget* createRegisterTab();

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog() override = default;

    UserPtr getAuthenticatedUser() const { return m_authenticatedUser; }

private slots:
    void handleStudentLogin();
    void handleAdminLogin();
    void handleRegister();
};

#endif // LOGIN_DIALOG_H
