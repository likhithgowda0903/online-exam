cmake_minimum_required(VERSION 3.16)
project(OnlineExamSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 components, fallback to Qt5 if Qt6 is not present
find_package(Qt6 COMPONENTS Widgets Core Gui Sql QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Core Gui Sql REQUIRED)
    set(QT_LIBRARIES Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Sql)
else()
    set(QT_LIBRARIES Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Sql)
endif()

include_directories(include)

set(PROJECT_SOURCES
    src/main.cpp
    src/models/User.cpp
    src/models/Student.cpp
    src/models/Admin.cpp
    src/models/Question.cpp
    src/models/Quiz.cpp
    src/models/Result.cpp
    src/models/ExamSession.cpp
    src/database/PasswordHasher.cpp
    src/database/DatabaseManager.cpp
    src/core/ExamTimer.cpp
    src/core/Evaluator.cpp
    src/core/QuestionBank.cpp
    src/ui/LoginDialog.cpp
    src/ui/StudentDashboard.cpp
    src/ui/AdminDashboard.cpp
    src/ui/ExamWindow.cpp
    src/ui/QuizEditorDialog.cpp
    src/ui/QuestionEditorDialog.cpp
    src/ui/ResultDialog.cpp
    src/ui/MainWindow.cpp
    resources.qrc
)

add_executable(OnlineExamSystem ${PROJECT_SOURCES})
target_link_libraries(OnlineExamSystem PRIVATE ${QT_LIBRARIES})

# Copy styles to binary directory on build
add_custom_command(TARGET OnlineExamSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/styles
    $<TARGET_FILE_DIR:OnlineExamSystem>/styles
)

# Test Executable
set(TEST_SOURCES
    tests/test_exam_system.cpp
    src/models/User.cpp
    src/models/Student.cpp
    src/models/Admin.cpp
    src/models/Question.cpp
    src/models/Quiz.cpp
    src/models/Result.cpp
    src/models/ExamSession.cpp
    src/database/PasswordHasher.cpp
    src/core/Evaluator.cpp
    src/core/QuestionBank.cpp
)

add_executable(test_exam_system ${TEST_SOURCES})
target_link_libraries(test_exam_system PRIVATE ${QT_LIBRARIES})
