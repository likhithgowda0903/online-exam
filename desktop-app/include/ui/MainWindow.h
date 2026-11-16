#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "models/User.h"
#include "models/Student.h"
#include "models/Admin.h"

/**
 * @file MainWindow.h
 * @brief Host main window with dynamic polymorphic dashboard switching.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    UserPtr m_currentUser;
    QStackedWidget* m_stackedWidget;

    void setupUI();
    void showLogin();
    void loadUserDashboard();

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

public slots:
    void handleLogout();
};

#endif // MAIN_WINDOW_H
