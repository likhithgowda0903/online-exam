QT       += core gui widgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = OnlineExamSystem
TEMPLATE = app

INCLUDEPATH += include

HEADERS += \
    include/models/User.h \
    include/models/Student.h \
    include/models/Admin.h \
    include/models/Question.h \
    include/models/Quiz.h \
    include/models/Result.h \
    include/models/ExamSession.h \
    include/database/PasswordHasher.h \
    include/database/DatabaseManager.h \
    include/core/ExamTimer.h \
    include/core/Evaluator.h \
    include/core/QuestionBank.h \
    include/ui/LoginDialog.h \
    include/ui/StudentDashboard.h \
    include/ui/AdminDashboard.h \
    include/ui/ExamWindow.h \
    include/ui/QuizEditorDialog.h \
    include/ui/QuestionEditorDialog.h \
    include/ui/ResultDialog.h \
    include/ui/MainWindow.h

SOURCES += \
    src/main.cpp \
    src/models/User.cpp \
    src/models/Student.cpp \
    src/models/Admin.cpp \
    src/models/Question.cpp \
    src/models/Quiz.cpp \
    src/models/Result.cpp \
    src/models/ExamSession.cpp \
    src/database/PasswordHasher.cpp \
    src/database/DatabaseManager.cpp \
    src/core/ExamTimer.cpp \
    src/core/Evaluator.cpp \
    src/core/QuestionBank.cpp \
    src/ui/LoginDialog.cpp \
    src/ui/StudentDashboard.cpp \
    src/ui/AdminDashboard.cpp \
    src/ui/ExamWindow.cpp \
    src/ui/QuizEditorDialog.cpp \
    src/ui/QuestionEditorDialog.cpp \
    src/ui/ResultDialog.cpp \
    src/ui/MainWindow.cpp

RESOURCES += \
    resources.qrc
