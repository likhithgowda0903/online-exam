#include <QApplication>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include "database/DatabaseManager.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Online Examination System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MCA Mini Project");

    // Initialize SQLite Database with complete schema and sample quizzes
    QString dbPath = "online_exam.db";
    if (!DatabaseManager::instance().initDatabase(dbPath)) {
        QMessageBox::critical(nullptr, "Database Error", 
            "Failed to initialize SQLite database. Ensure write permissions exist in the application directory.");
        return 1;
    }

    // Load and apply Modern QSS stylesheet
    QString qssPath = ":/styles/modern_theme.qss";
    if (!QFile::exists(qssPath)) {
        // Fallback to local file path if resources.qrc isn't compiled
        qssPath = QDir::current().filePath("styles/modern_theme.qss");
        if (!QFile::exists(qssPath)) {
            qssPath = "../styles/modern_theme.qss";
        }
    }

    QFile qssFile(qssPath);
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QString::fromUtf8(qssFile.readAll());
        app.setStyleSheet(styleSheet);
        qssFile.close();
    } else {
        qWarning() << "Could not open stylesheet file at:" << qssPath << "- Using default dark fallback.";
        app.setStyleSheet("QWidget { background-color: #0f172a; color: #f8fafc; font-family: Segoe UI, sans-serif; }");
    }

    // Launch Main Application Window
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
