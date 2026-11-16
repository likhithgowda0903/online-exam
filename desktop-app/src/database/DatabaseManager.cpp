#include "database/DatabaseManager.h"
#include "database/PasswordHasher.h"
#include "core/QuestionBank.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QWidget>
#include <QPushButton>

DatabaseManager::DatabaseManager() : m_isInitialized(false) {
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initDatabase(const QString& dbPath) {
    if (m_isInitialized && m_db.isOpen()) {
        return true;
    }

    m_dbPath = dbPath;

    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "Failed to open SQLite database:" << m_db.lastError().text();
        return false;
    }

    // Enable foreign keys
    QSqlQuery pragmaQuery(m_db);
    pragmaQuery.exec("PRAGMA foreign_keys = ON;");

    if (!createTables()) {
        return false;
    }

    m_isInitialized = true;
    seedDefaultData();
    return true;
}

bool DatabaseManager::isConnected() const {
    return m_isInitialized && m_db.isOpen();
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_isInitialized = false;
}

QString DatabaseManager::getDatabasePath() const {
    if (m_db.isOpen() && !m_db.databaseName().isEmpty()) {
        QFileInfo fi(m_db.databaseName());
        if (fi.exists()) {
            return fi.absoluteFilePath();
        }
    }
    if (!m_dbPath.isEmpty()) {
        QFileInfo fi(m_dbPath);
        if (fi.exists()) {
            return fi.absoluteFilePath();
        }
    }
    QStringList candidates = {
        "online_exam.db",
        QCoreApplication::applicationDirPath() + "/online_exam.db",
        "../online_exam.db",
        "../../online_exam.db"
    };
    for (const QString& cand : candidates) {
        QFileInfo fi(cand);
        if (fi.exists()) {
            return fi.absoluteFilePath();
        }
    }
    return QFileInfo("online_exam.db").absoluteFilePath();
}

QString DatabaseManager::findDbBrowserExecutable() const {
    // 1. Standard 64-bit and 32-bit Program Files locations on Windows
    QStringList searchPaths = {
        "C:/Program Files/DB Browser for SQLite/DB Browser for SQLite.exe",
        "C:/Program Files (x86)/DB Browser for SQLite/DB Browser for SQLite.exe"
    };

    QString localApp = qgetenv("LOCALAPPDATA");
    if (!localApp.isEmpty()) {
        searchPaths << localApp + "/Programs/DB Browser for SQLite/DB Browser for SQLite.exe";
    }

    for (const QString& path : searchPaths) {
        if (QFileInfo::exists(path)) {
            return path;
        }
    }

    // 2. Search system PATH using QStandardPaths
    QString exe = QStandardPaths::findExecutable("DB Browser for SQLite.exe");
    if (!exe.isEmpty()) return exe;

    exe = QStandardPaths::findExecutable("sqlitebrowser.exe");
    if (!exe.isEmpty()) return exe;

    exe = QStandardPaths::findExecutable("sqlitebrowser");
    if (!exe.isEmpty()) return exe;

    return QString();
}

bool DatabaseManager::openInDbBrowser(QWidget* parentWidget) {
    QString absDbPath = QDir::toNativeSeparators(getDatabasePath());
    QString dbBrowserExe = findDbBrowserExecutable();

    if (!dbBrowserExe.isEmpty()) {
        // Launch DB Browser for SQLite on Windows using QProcess
        QStringList args;
        args << absDbPath;
        bool started = QProcess::startDetached(dbBrowserExe, args);
        if (started) {
            return true;
        }
    }

    // If DB Browser for SQLite is not installed:
    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle("Open Database - DB Browser for SQLite");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("<h3>DB Browser for SQLite Not Installed</h3>");
    msgBox.setInformativeText(
        QString("<p>This project uses <b>SQLite 3</b>. Your actual database file is located at:</p>"
                "<p style='font-family: Consolas, monospace; background: #0f172a; color: #38bdf8; padding: 8px; border: 1px solid #334155; border-radius: 4px;'><b>%1</b></p>"
                "<p><b>Active Database Tables:</b> <code>users</code>, <code>quizzes</code>, <code>questions</code>, <code>results</code></p>"
                "<hr/>"
                "<p><b>To view and inspect your real stored tables and records:</b></p>"
                "<ol style='margin-left: 10px;'>"
                "<li>Download <b>DB Browser for SQLite</b> from: <br/><a href='https://sqlitebrowser.org/dl/'>https://sqlitebrowser.org/dl/</a></li>"
                "<li>Run the Windows installer.</li>"
                "<li>Click <b>'Open Database'</b> again, and your database will launch automatically!</li>"
                "</ol>")
        .arg(absDbPath));

    auto* openExplorerBtn = msgBox.addButton("📁 Show Database in Explorer", QMessageBox::ActionRole);
    auto* downloadBtn = msgBox.addButton("📥 Download DB Browser", QMessageBox::ActionRole);
    msgBox.addButton(QMessageBox::Close);

    msgBox.exec();

    if (msgBox.clickedButton() == openExplorerBtn) {
        // Highlight the real online_exam.db file in Windows File Explorer
        QProcess::startDetached("explorer.exe", QStringList() << QString("/select,%1").arg(absDbPath));
    } else if (msgBox.clickedButton() == downloadBtn) {
        // Open the official download site in the default web browser
        QDesktopServices::openUrl(QUrl("https://sqlitebrowser.org/dl/"));
    }

    return false;
}

bool DatabaseManager::createTables() {
    QSqlQuery query(m_db);

    // Users Table
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            salt TEXT NOT NULL,
            full_name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            role TEXT NOT NULL,
            specific_id TEXT NOT NULL,
            created_at TEXT NOT NULL
        );
    )";
    if (!query.exec(createUsersTable)) {
        qCritical() << "Error creating users table:" << query.lastError().text();
        return false;
    }

    // Quizzes Table
    QString createQuizzesTable = R"(
        CREATE TABLE IF NOT EXISTS quizzes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            category TEXT NOT NULL,
            description TEXT,
            duration_minutes INTEGER NOT NULL,
            pass_percentage REAL NOT NULL,
            total_marks INTEGER NOT NULL DEFAULT 0,
            created_at TEXT NOT NULL
        );
    )";
    if (!query.exec(createQuizzesTable)) {
        qCritical() << "Error creating quizzes table:" << query.lastError().text();
        return false;
    }

    // Questions Table
    QString createQuestionsTable = R"(
        CREATE TABLE IF NOT EXISTS questions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quiz_id INTEGER NOT NULL,
            question_text TEXT NOT NULL,
            option_a TEXT NOT NULL,
            option_b TEXT NOT NULL,
            option_c TEXT NOT NULL,
            option_d TEXT NOT NULL,
            correct_option INTEGER NOT NULL,
            marks INTEGER NOT NULL DEFAULT 1,
            explanation TEXT,
            FOREIGN KEY (quiz_id) REFERENCES quizzes(id) ON DELETE CASCADE
        );
    )";
    if (!query.exec(createQuestionsTable)) {
        qCritical() << "Error creating questions table:" << query.lastError().text();
        return false;
    }

    // Results Table
    QString createResultsTable = R"(
        CREATE TABLE IF NOT EXISTS results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            quiz_id INTEGER NOT NULL,
            total_questions INTEGER NOT NULL,
            correct_count INTEGER NOT NULL,
            incorrect_count INTEGER NOT NULL,
            unanswered_count INTEGER NOT NULL,
            total_marks INTEGER NOT NULL,
            marks_obtained INTEGER NOT NULL,
            percentage REAL NOT NULL,
            is_passed INTEGER NOT NULL,
            attempt_date TEXT NOT NULL,
            time_taken_seconds INTEGER NOT NULL,
            FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
            FOREIGN KEY (quiz_id) REFERENCES quizzes(id) ON DELETE CASCADE
        );
    )";
    if (!query.exec(createResultsTable)) {
        qCritical() << "Error creating results table:" << query.lastError().text();
        return false;
    }

    return true;
}

void DatabaseManager::seedDefaultData() {
    // All demo users, demo administrator, and sample questions have been removed.
    // User accounts and examination question pools are created dynamically via
    // registration and the administrator portal.
}

bool DatabaseManager::registerStudent(const QString& username, const QString& password,
                                      const QString& fullName, const QString& email,
                                      const QString& studentId, QString& errorMsg) {
    if (isUsernameTaken(username)) {
        errorMsg = "Username is already registered. Please choose another username.";
        return false;
    }
    if (isEmailTaken(email)) {
        errorMsg = "Email address is already registered. Please use another email.";
        return false;
    }
    if (isStudentIdTaken(studentId)) {
        errorMsg = "Student ID already exists in the system.";
        return false;
    }

    QString salt = PasswordHasher::generateSalt();
    QString hash = PasswordHasher::hashPassword(password, salt);

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO users (username, password_hash, salt, full_name, email, role, specific_id, created_at)
        VALUES (:username, :hash, :salt, :fullName, :email, 'Student', :sid, :created);
    )");
    query.bindValue(":username", username.trimmed());
    query.bindValue(":hash", hash);
    query.bindValue(":salt", salt);
    query.bindValue(":fullName", fullName.trimmed());
    query.bindValue(":email", email.trimmed());
    query.bindValue(":sid", studentId.trimmed());
    query.bindValue(":created", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        errorMsg = "Database error: " + query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::registerAdmin(const QString& username, const QString& password,
                                    const QString& fullName, const QString& email,
                                    const QString& adminId, QString& errorMsg) {
    if (isUsernameTaken(username)) {
        errorMsg = "Username is already registered. Please choose another username.";
        return false;
    }
    if (isEmailTaken(email)) {
        errorMsg = "Email address is already registered. Please use another email.";
        return false;
    }

    QString salt = PasswordHasher::generateSalt();
    QString hash = PasswordHasher::hashPassword(password, salt);

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO users (username, password_hash, salt, full_name, email, role, specific_id, created_at)
        VALUES (:username, :hash, :salt, :fullName, :email, 'Admin', :aid, :created);
    )");
    query.bindValue(":username", username.trimmed());
    query.bindValue(":hash", hash);
    query.bindValue(":salt", salt);
    query.bindValue(":fullName", fullName.trimmed());
    query.bindValue(":email", email.trimmed());
    query.bindValue(":aid", adminId.trimmed());
    query.bindValue(":created", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        errorMsg = "Database error: " + query.lastError().text();
        return false;
    }

    return true;
}

UserPtr DatabaseManager::authenticateUser(const QString& username, const QString& password, QString& errorMsg) {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, username, password_hash, salt, full_name, email, role, specific_id, created_at FROM users WHERE username = :username;");
    query.bindValue(":username", username.trimmed());

    if (!query.exec() || !query.next()) {
        errorMsg = "Invalid username or account not found.";
        return nullptr;
    }

    int id = query.value("id").toInt();
    QString uname = query.value("username").toString();
    QString storedHash = query.value("password_hash").toString();
    QString salt = query.value("salt").toString();
    QString fullName = query.value("full_name").toString();
    QString email = query.value("email").toString();
    QString role = query.value("role").toString();
    QString specificId = query.value("specific_id").toString();
    QDateTime created = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);

    if (!PasswordHasher::verifyPassword(password, storedHash, salt)) {
        errorMsg = "Incorrect password. Please try again.";
        return nullptr;
    }

    // Polymorphic instantiation based on role
    if (role == "Admin") {
        return std::make_shared<Admin>(id, uname, fullName, email, specificId, "Academic Affairs", created);
    } else {
        int totalExams = getStudentCompletedExamsCount(id);
        double avgScore = getStudentAverageScore(id);
        return std::make_shared<Student>(id, uname, fullName, email, specificId, totalExams, avgScore, created);
    }
}

bool DatabaseManager::isUsernameTaken(const QString& username) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE LOWER(username) = LOWER(:username);");
    query.bindValue(":username", username.trimmed());
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::isEmailTaken(const QString& email) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE LOWER(email) = LOWER(:email);");
    query.bindValue(":email", email.trimmed());
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::isStudentIdTaken(const QString& studentId) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE LOWER(specific_id) = LOWER(:sid);");
    query.bindValue(":sid", studentId.trimmed());
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

std::vector<Student> DatabaseManager::getAllStudents() {
    std::vector<Student> students;
    QSqlQuery query(m_db);
    query.exec("SELECT id, username, full_name, email, specific_id, created_at FROM users WHERE role = 'Student' ORDER BY id DESC;");
    while (query.next()) {
        int id = query.value("id").toInt();
        QString uname = query.value("username").toString();
        QString name = query.value("full_name").toString();
        QString email = query.value("email").toString();
        QString sid = query.value("specific_id").toString();
        QDateTime dt = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);

        int totalExams = getStudentCompletedExamsCount(id);
        double avg = getStudentAverageScore(id);

        students.emplace_back(id, uname, name, email, sid, totalExams, avg, dt);
    }
    return students;
}

int DatabaseManager::getTotalStudentsCount() {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM users WHERE role = 'Student';");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool DatabaseManager::deleteStudent(int userId) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = :id AND role = 'Student';");
    query.bindValue(":id", userId);
    return query.exec();
}

int DatabaseManager::saveQuiz(const Quiz& quiz, QString& errorMsg) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO quizzes (title, category, description, duration_minutes, pass_percentage, total_marks, created_by, created_at)
        VALUES (:title, :cat, :desc, :dur, :pass, :total, :created_by, :created);
    )");
    query.bindValue(":title", quiz.getTitle());
    query.bindValue(":cat", quiz.getCategory());
    query.bindValue(":desc", quiz.getDescription());
    query.bindValue(":dur", quiz.getDurationMinutes());
    query.bindValue(":pass", quiz.getPassPercentage());
    query.bindValue(":total", quiz.getTotalMarks());
    query.bindValue(":created_by", quiz.getCreatedBy());
    query.bindValue(":created", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        errorMsg = query.lastError().text();
        return -1;
    }

    int quizId = query.lastInsertId().toInt();

    // Insert associated questions if any
    for (const auto& q : quiz.getQuestions()) {
        Question qCopy = q;
        qCopy.setQuizId(quizId);
        saveQuestion(qCopy, errorMsg);
    }

    return quizId;
}

bool DatabaseManager::updateQuiz(const Quiz& quiz, QString& errorMsg) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE quizzes 
        SET title = :title, category = :cat, description = :desc, 
            duration_minutes = :dur, pass_percentage = :pass, total_marks = :total, created_by = :created_by
        WHERE id = :id;
    )");
    query.bindValue(":title", quiz.getTitle());
    query.bindValue(":cat", quiz.getCategory());
    query.bindValue(":desc", quiz.getDescription());
    query.bindValue(":dur", quiz.getDurationMinutes());
    query.bindValue(":pass", quiz.getPassPercentage());
    query.bindValue(":total", quiz.getTotalMarks());
    query.bindValue(":created_by", quiz.getCreatedBy());
    query.bindValue(":id", quiz.getId());

    if (!query.exec()) {
        errorMsg = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteQuiz(int quizId) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM quizzes WHERE id = :id;");
    query.bindValue(":id", quizId);
    return query.exec();
}

std::vector<Quiz> DatabaseManager::getAllQuizzes(bool loadQuestions) {
    std::vector<Quiz> quizzes;
    QSqlQuery query(m_db);
    query.exec("SELECT id, title, category, description, duration_minutes, pass_percentage, total_marks, created_by FROM quizzes ORDER BY id DESC;");
    while (query.next()) {
        int id = query.value("id").toInt();
        QString title = query.value("title").toString();
        QString cat = query.value("category").toString();
        QString desc = query.value("description").toString();
        int dur = query.value("duration_minutes").toInt();
        double pass = query.value("pass_percentage").toDouble();
        int total = query.value("total_marks").toInt();
        QString creator = query.value("created_by").toString();

        Quiz q(id, title, cat, desc, dur, pass, total, creator);
        if (loadQuestions) {
            q.setQuestions(getQuestionsForQuiz(id));
        }
        quizzes.push_back(q);
    }
    return quizzes;
}

std::vector<Quiz> DatabaseManager::getQuizzesByAdmin(const QString& adminId, bool loadQuestions) {
    std::vector<Quiz> quizzes;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, title, category, description, duration_minutes, pass_percentage, total_marks, created_by FROM quizzes WHERE LOWER(created_by) = LOWER(:adminId) ORDER BY id DESC;");
    query.bindValue(":adminId", adminId.trimmed());
    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            QString title = query.value("title").toString();
            QString cat = query.value("category").toString();
            QString desc = query.value("description").toString();
            int dur = query.value("duration_minutes").toInt();
            double pass = query.value("pass_percentage").toDouble();
            int total = query.value("total_marks").toInt();
            QString creator = query.value("created_by").toString();

            Quiz q(id, title, cat, desc, dur, pass, total, creator);
            if (loadQuestions) {
                q.setQuestions(getQuestionsForQuiz(id));
            }
            quizzes.push_back(q);
        }
    }
    return quizzes;
}

int DatabaseManager::getTotalQuizzesCountByAdmin(const QString& adminId) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM quizzes WHERE LOWER(created_by) = LOWER(:adminId);");
    query.bindValue(":adminId", adminId.trimmed());
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

std::optional<Quiz> DatabaseManager::getQuizById(int quizId, bool loadQuestions) {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, title, category, description, duration_minutes, pass_percentage, total_marks FROM quizzes WHERE id = :id;");
    query.bindValue(":id", quizId);
    if (query.exec() && query.next()) {
        int id = query.value("id").toInt();
        QString title = query.value("title").toString();
        QString cat = query.value("category").toString();
        QString desc = query.value("description").toString();
        int dur = query.value("duration_minutes").toInt();
        double pass = query.value("pass_percentage").toDouble();
        int total = query.value("total_marks").toInt();

        Quiz q(id, title, cat, desc, dur, pass, total);
        if (loadQuestions) {
            q.setQuestions(getQuestionsForQuiz(id));
        }
        return q;
    }
    return std::nullopt;
}

int DatabaseManager::getTotalQuizzesCount() {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM quizzes;");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::saveQuestion(const Question& question, QString& errorMsg) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO questions (quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation)
        VALUES (:qid, :text, :oa, :ob, :oc, :od, :correct, :marks, :exp);
    )");
    query.bindValue(":qid", question.getQuizId());
    query.bindValue(":text", question.getQuestionText());
    query.bindValue(":oa", question.getOption(0));
    query.bindValue(":ob", question.getOption(1));
    query.bindValue(":oc", question.getOption(2));
    query.bindValue(":od", question.getOption(3));
    query.bindValue(":correct", question.getCorrectOptionIndex());
    query.bindValue(":marks", question.getMarks());
    query.bindValue(":exp", question.getExplanation());

    if (!query.exec()) {
        errorMsg = query.lastError().text();
        return -1;
    }

    // Recalculate total marks for the quiz
    QSqlQuery sumQ(m_db);
    sumQ.prepare("SELECT SUM(marks) FROM questions WHERE quiz_id = :qid;");
    sumQ.bindValue(":qid", question.getQuizId());
    if (sumQ.exec() && sumQ.next()) {
        int total = sumQ.value(0).toInt();
        QSqlQuery updateMarks(m_db);
        updateMarks.prepare("UPDATE quizzes SET total_marks = :total WHERE id = :qid;");
        updateMarks.bindValue(":total", total);
        updateMarks.bindValue(":qid", question.getQuizId());
        updateMarks.exec();
    }

    return query.lastInsertId().toInt();
}

bool DatabaseManager::updateQuestion(const Question& question, QString& errorMsg) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE questions
        SET question_text = :text, option_a = :oa, option_b = :ob, option_c = :oc, option_d = :od,
            correct_option = :correct, marks = :marks, explanation = :exp
        WHERE id = :id;
    )");
    query.bindValue(":text", question.getQuestionText());
    query.bindValue(":oa", question.getOption(0));
    query.bindValue(":ob", question.getOption(1));
    query.bindValue(":oc", question.getOption(2));
    query.bindValue(":od", question.getOption(3));
    query.bindValue(":correct", question.getCorrectOptionIndex());
    query.bindValue(":marks", question.getMarks());
    query.bindValue(":exp", question.getExplanation());
    query.bindValue(":id", question.getId());

    if (!query.exec()) {
        errorMsg = query.lastError().text();
        return false;
    }

    // Recalculate total marks for the quiz
    QSqlQuery sumQ(m_db);
    sumQ.prepare("SELECT SUM(marks) FROM questions WHERE quiz_id = :qid;");
    sumQ.bindValue(":qid", question.getQuizId());
    if (sumQ.exec() && sumQ.next()) {
        int total = sumQ.value(0).toInt();
        QSqlQuery updateMarks(m_db);
        updateMarks.prepare("UPDATE quizzes SET total_marks = :total WHERE id = :qid;");
        updateMarks.bindValue(":total", total);
        updateMarks.bindValue(":qid", question.getQuizId());
        updateMarks.exec();
    }

    return true;
}

bool DatabaseManager::deleteQuestion(int questionId) {
    int quizId = 0;
    QSqlQuery qIdFind(m_db);
    qIdFind.prepare("SELECT quiz_id FROM questions WHERE id = :id;");
    qIdFind.bindValue(":id", questionId);
    if (qIdFind.exec() && qIdFind.next()) {
        quizId = qIdFind.value(0).toInt();
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM questions WHERE id = :id;");
    query.bindValue(":id", questionId);
    bool ok = query.exec();

    if (ok && quizId > 0) {
        QSqlQuery sumQ(m_db);
        sumQ.prepare("SELECT COALESCE(SUM(marks), 0) FROM questions WHERE quiz_id = :qid;");
        sumQ.bindValue(":qid", quizId);
        if (sumQ.exec() && sumQ.next()) {
            int total = sumQ.value(0).toInt();
            QSqlQuery updateMarks(m_db);
            updateMarks.prepare("UPDATE quizzes SET total_marks = :total WHERE id = :qid;");
            updateMarks.bindValue(":total", total);
            updateMarks.bindValue(":qid", quizId);
            updateMarks.exec();
        }
    }

    return ok;
}

std::vector<Question> DatabaseManager::getQuestionsForQuiz(int quizId) {
    std::vector<Question> questions;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation FROM questions WHERE quiz_id = :qid ORDER BY id ASC;");
    query.bindValue(":qid", quizId);
    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            int qid = query.value("quiz_id").toInt();
            QString text = query.value("question_text").toString();
            QStringList opts = {
                query.value("option_a").toString(),
                query.value("option_b").toString(),
                query.value("option_c").toString(),
                query.value("option_d").toString()
            };
            int correct = query.value("correct_option").toInt();
            int marks = query.value("marks").toInt();
            QString exp = query.value("explanation").toString();

            questions.emplace_back(id, qid, text, opts, correct, marks, exp);
        }
    }
    return questions;
}

std::vector<Question> DatabaseManager::getDynamicQuestionsForQuiz(int quizId, int count, bool shuffleOptions) {
    std::vector<Question> allQuestions = getQuestionsForQuiz(quizId);
    if (allQuestions.empty()) return {};

    RandomizedQuestionSelector selector(shuffleOptions);
    return selector.selectQuestions(allQuestions, count);
}

int DatabaseManager::saveResult(const Result& result, QString& errorMsg) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO results (student_id, quiz_id, total_questions, correct_count, incorrect_count, unanswered_count, total_marks, marks_obtained, percentage, is_passed, attempt_date, time_taken_seconds)
        VALUES (:sid, :qid, :totalQ, :cCount, :iCount, :uCount, :tMarks, :mObtained, :pct, :passed, :date, :time);
    )");
    query.bindValue(":sid", result.getStudentId());
    query.bindValue(":qid", result.getQuizId());
    query.bindValue(":totalQ", result.getTotalQuestions());
    query.bindValue(":cCount", result.getCorrectAnswers());
    query.bindValue(":iCount", result.getIncorrectAnswers());
    query.bindValue(":uCount", result.getUnansweredQuestions());
    query.bindValue(":tMarks", result.getTotalMarks());
    query.bindValue(":mObtained", result.getMarksObtained());
    query.bindValue(":pct", result.getPercentage());
    query.bindValue(":passed", result.isPassed() ? 1 : 0);
    query.bindValue(":date", result.getAttemptDate().toString("yyyy-MM-dd hh:mm AP"));
    query.bindValue(":time", result.getTimeTakenSeconds());

    if (!query.exec()) {
        errorMsg = query.lastError().text();
        return -1;
    }
    return query.lastInsertId().toInt();
}

std::vector<Result> DatabaseManager::getResultsForStudent(int studentId) {
    std::vector<Result> list;
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT r.id, r.student_id, r.quiz_id, r.total_questions, r.correct_count, r.incorrect_count, 
               r.unanswered_count, r.total_marks, r.marks_obtained, r.percentage, r.is_passed, 
               r.attempt_date, r.time_taken_seconds,
               u.username as student_uname, u.full_name as student_fname,
               q.title as quiz_title
        FROM results r
        JOIN users u ON r.student_id = u.id
        JOIN quizzes q ON r.quiz_id = q.id
        WHERE r.student_id = :sid
        ORDER BY r.id DESC;
    )");
    query.bindValue(":sid", studentId);
    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            int sid = query.value("student_id").toInt();
            int qid = query.value("quiz_id").toInt();
            int tq = query.value("total_questions").toInt();
            int cc = query.value("correct_count").toInt();
            int ic = query.value("incorrect_count").toInt();
            int uc = query.value("unanswered_count").toInt();
            int tm = query.value("total_marks").toInt();
            int mo = query.value("marks_obtained").toInt();
            double pct = query.value("percentage").toDouble();
            bool passed = query.value("is_passed").toInt() == 1;
            QDateTime dt = QDateTime::fromString(query.value("attempt_date").toString(), "yyyy-MM-dd hh:mm AP");
            int time = query.value("time_taken_seconds").toInt();
            QString sUname = query.value("student_uname").toString();
            QString sFname = query.value("student_fname").toString();
            QString qTitle = query.value("quiz_title").toString();

            list.emplace_back(id, sid, qid, tq, cc, ic, uc, tm, mo, pct, passed, dt, time, sUname, sFname, qTitle);
        }
    }
    return list;
}

std::vector<Result> DatabaseManager::getAllResults() {
    std::vector<Result> list;
    QSqlQuery query(m_db);
    QString sql = R"(
        SELECT r.id, r.student_id, r.quiz_id, r.total_questions, r.correct_count, r.incorrect_count, 
               r.unanswered_count, r.total_marks, r.marks_obtained, r.percentage, r.is_passed, 
               r.attempt_date, r.time_taken_seconds,
               u.username as student_uname, u.full_name as student_fname,
               q.title as quiz_title
        FROM results r
        JOIN users u ON r.student_id = u.id
        JOIN quizzes q ON r.quiz_id = q.id
        ORDER BY r.id DESC;
    )";
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value("id").toInt();
            int sid = query.value("student_id").toInt();
            int qid = query.value("quiz_id").toInt();
            int tq = query.value("total_questions").toInt();
            int cc = query.value("correct_count").toInt();
            int ic = query.value("incorrect_count").toInt();
            int uc = query.value("unanswered_count").toInt();
            int tm = query.value("total_marks").toInt();
            int mo = query.value("marks_obtained").toInt();
            double pct = query.value("percentage").toDouble();
            bool passed = query.value("is_passed").toInt() == 1;
            QDateTime dt = QDateTime::fromString(query.value("attempt_date").toString(), "yyyy-MM-dd hh:mm AP");
            int time = query.value("time_taken_seconds").toInt();
            QString sUname = query.value("student_uname").toString();
            QString sFname = query.value("student_fname").toString();
            QString qTitle = query.value("quiz_title").toString();

            list.emplace_back(id, sid, qid, tq, cc, ic, uc, tm, mo, pct, passed, dt, time, sUname, sFname, qTitle);
        }
    }
    return list;
}

std::optional<Result> DatabaseManager::getResultById(int resultId) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT r.id, r.student_id, r.quiz_id, r.total_questions, r.correct_count, r.incorrect_count, 
               r.unanswered_count, r.total_marks, r.marks_obtained, r.percentage, r.is_passed, 
               r.attempt_date, r.time_taken_seconds,
               u.username as student_uname, u.full_name as student_fname,
               q.title as quiz_title
        FROM results r
        JOIN users u ON r.student_id = u.id
        JOIN quizzes q ON r.quiz_id = q.id
        WHERE r.id = :id;
    )");
    query.bindValue(":id", resultId);
    if (query.exec() && query.next()) {
        int id = query.value("id").toInt();
        int sid = query.value("student_id").toInt();
        int qid = query.value("quiz_id").toInt();
        int tq = query.value("total_questions").toInt();
        int cc = query.value("correct_count").toInt();
        int ic = query.value("incorrect_count").toInt();
        int uc = query.value("unanswered_count").toInt();
        int tm = query.value("total_marks").toInt();
        int mo = query.value("marks_obtained").toInt();
        double pct = query.value("percentage").toDouble();
        bool passed = query.value("is_passed").toInt() == 1;
        QDateTime dt = QDateTime::fromString(query.value("attempt_date").toString(), "yyyy-MM-dd hh:mm AP");
        int time = query.value("time_taken_seconds").toInt();
        QString sUname = query.value("student_uname").toString();
        QString sFname = query.value("student_fname").toString();
        QString qTitle = query.value("quiz_title").toString();

        return Result(id, sid, qid, tq, cc, ic, uc, tm, mo, pct, passed, dt, time, sUname, sFname, qTitle);
    }
    return std::nullopt;
}

int DatabaseManager::getTotalExamsCompletedCount() {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM results;");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

double DatabaseManager::getGlobalAverageScore() {
    QSqlQuery query(m_db);
    query.exec("SELECT AVG(percentage) FROM results;");
    if (query.next() && !query.value(0).isNull()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

double DatabaseManager::getStudentAverageScore(int studentId) {
    QSqlQuery query(m_db);
    query.prepare("SELECT AVG(percentage) FROM results WHERE student_id = :sid;");
    query.bindValue(":sid", studentId);
    if (query.exec() && query.next() && !query.value(0).isNull()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

int DatabaseManager::getStudentCompletedExamsCount(int studentId) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM results WHERE student_id = :sid;");
    query.bindValue(":sid", studentId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::getStudentPassedExamsCount(int studentId) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM results WHERE student_id = :sid AND is_passed = 1;");
    query.bindValue(":sid", studentId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
