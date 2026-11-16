# 📊 Online Examination System - UML Diagrams & System Architecture

This document contains complete UML design models representing the Object-Oriented architecture of the Online Examination System mini project.

---

## 1. Class Diagram (OOP Design)

```mermaid
classDiagram
    %% Abstract Base Class
    class User {
        <<abstract>>
        #int m_id
        #QString m_username
        #QString m_fullName
        #QString m_email
        #QString m_role
        #QDateTime m_createdAt
        +User()
        +User(id, username, fullName, email, role, createdAt)
        +virtual ~User()
        +virtual QString getRole()*
        +virtual QString getSpecificId()
        +virtual QString getDisplayName()
        +int getId()
        +QString getUsername()
        +QString getFullName()
        +QString getEmail()
        +void setFullName(QString)
        +void setEmail(QString)
    }

    %% Derived Student Class
    class Student {
        -QString m_studentId
        -int m_totalExamsTaken
        -double m_averageScore
        +Student()
        +Student(id, username, fullName, email, studentId, totalExams, avgScore, createdAt)
        +QString getRole() override
        +QString getSpecificId() override
        +QString getStudentId()
        +int getTotalExamsTaken()
        +double getAverageScore()
    }

    %% Derived Admin Class
    class Admin {
        -QString m_adminCode
        -QString m_department
        +Admin()
        +Admin(id, username, fullName, email, adminCode, department, createdAt)
        +QString getRole() override
        +QString getSpecificId() override
        +QString getAdminCode()
        +QString getDepartment()
    }

    %% Question Class
    class Question {
        -int m_id
        -int m_quizId
        -QString m_questionText
        -QStringList m_options
        -int m_correctOptionIndex
        -int m_marks
        -QString m_explanation
        +Question()
        +Question(id, quizId, text, options, correctIndex, marks, explanation)
        +QString getOption(int index)
        +bool isCorrect(int selectedIndex)
        +bool isValid()
        +int getMarks()
    }

    %% Quiz Class
    class Quiz {
        -int m_id
        -QString m_title
        -QString m_category
        -QString m_description
        -int m_durationMinutes
        -double m_passPercentage
        -int m_totalMarks
        -vector~Question~ m_questions
        +Quiz()
        +void addQuestion(Question)
        +void removeQuestion(int id)
        +const Question* getQuestionAt(int index)
        +void recalculateTotalMarks()
    }

    %% ExamSession Class
    class ExamSession {
        -int m_studentId
        -Quiz m_quiz
        -map~int, int~ m_userAnswers
        -set~int~ m_markedForReview
        -int m_currentQuestionIndex
        -int m_remainingSeconds
        -bool m_isSubmitted
        +void selectAnswer(int qIndex, int optIndex)
        +void clearAnswer(int qIndex)
        +void toggleMarkForReview(int qIndex)
        +int getAnsweredCount()
        +int getUnansweredCount()
        +void decrementTimer()
        +bool isTimeExpired()
    }

    %% Evaluator Interface and Strategy
    class IEvaluationStrategy {
        <<interface>>
        +virtual ~IEvaluationStrategy()
        +virtual Result evaluate(ExamSession session)*
    }

    class StandardEvaluator {
        +Result evaluate(ExamSession session) override
    }

    %% Result Class
    class Result {
        -int m_id
        -int m_studentId
        -int m_quizId
        -int m_totalQuestions
        -int m_correctAnswers
        -int m_incorrectAnswers
        -int m_unansweredQuestions
        -int m_totalMarks
        -int m_marksObtained
        -double m_percentage
        -bool m_isPassed
        -QDateTime m_attemptDate
        -int m_timeTakenSeconds
        +QString getFormattedDate()
        +QString getFormattedTimeTaken()
        +QString getStatusString()
    }

    %% Singleton Database Manager
    class DatabaseManager {
        -QSqlDatabase m_db
        -bool m_isInitialized
        -DatabaseManager()
        +static DatabaseManager& instance()
        +bool initDatabase(QString)
        +void seedDefaultData()
        +bool registerStudent(...)
        +UserPtr authenticateUser(username, password, errorMsg)
        +int saveQuiz(Quiz, errorMsg)
        +int saveResult(Result, errorMsg)
    }

    %% Password Hasher Utility
    class PasswordHasher {
        +static QString generateSalt(int length)
        +static QString hashPassword(QString plainText, QString salt)
        +static bool verifyPassword(QString plainText, QString storedHash, QString salt)
    }

    %% Inheritance Relationships
    User <|-- Student : Inheritance
    User <|-- Admin : Inheritance
    IEvaluationStrategy <|-- StandardEvaluator : Realization / Strategy

    %% Aggregation & Association
    Quiz "1" *-- "many" Question : Aggregates
    ExamSession "1" o-- "1" Quiz : Manages
    ExamSession ..> Result : Evaluates to
    StandardEvaluator ..> ExamSession : Computes
    StandardEvaluator ..> Result : Produces
    DatabaseManager ..> User : Persists
    DatabaseManager ..> Quiz : Persists
    DatabaseManager ..> Result : Persists
    DatabaseManager ..> PasswordHasher : Uses
```

---

## 2. Sequence Diagram: Student Examination Flow

```mermaid
sequenceDiagram
    autonumber
    actor Student
    participant LoginDialog
    participant DatabaseManager
    participant StudentDashboard
    participant ExamWindow
    participant ExamTimer
    participant StandardEvaluator
    participant ResultDialog

    Student->>LoginDialog: Enter username & password
    LoginDialog->>DatabaseManager: authenticateUser(username, password)
    DatabaseManager-->>LoginDialog: Return authenticated Student object
    LoginDialog-->>StudentDashboard: Open Student Portal

    Student->>StudentDashboard: Click "Start Exam ▶" on Quiz
    StudentDashboard->>ExamWindow: Launch ExamWindow(ExamSession)
    ExamWindow->>ExamTimer: start(durationMinutes * 60)
    
    loop Every Second
        ExamTimer->>ExamWindow: tick(remainingSeconds)
        opt Remaining < 2 Minutes
            ExamTimer->>ExamWindow: warningLowTime()
            ExamWindow-->>Student: Display Low Time Warning Dialog
        end
    end

    loop Navigating & Answering
        Student->>ExamWindow: Select Option (Radio Button)
        ExamWindow->>ExamWindow: Update Question Palette (Green)
        Student->>ExamWindow: Click "Next ▶"
    end

    alt Manual Submission
        Student->>ExamWindow: Click "Finish & Submit Exam"
        ExamWindow-->>Student: Confirmation prompt
        Student->>ExamWindow: Confirm Submit
    else Timer Expires (0:00)
        ExamTimer->>ExamWindow: timeout()
        ExamWindow-->>Student: Auto-Submit Notification
    end

    ExamWindow->>ExamTimer: stop()
    ExamWindow->>StandardEvaluator: evaluate(ExamSession)
    StandardEvaluator-->>ExamWindow: Computed Result object
    ExamWindow->>DatabaseManager: saveResult(Result)
    ExamWindow->>ResultDialog: exec(Result, ExamSession)
    ResultDialog-->>Student: Display Official Scorecard & Review
    ResultDialog->>StudentDashboard: Refresh Dashboard Metrics & History
```

---

## 3. Database Entity-Relationship (ER) Diagram

```mermaid
erDiagram
    USERS {
        int id PK "INTEGER AUTOINCREMENT"
        string username "TEXT UNIQUE"
        string password_hash "TEXT (SHA-256)"
        string salt "TEXT (16-char random)"
        string full_name "TEXT"
        string email "TEXT UNIQUE"
        string role "TEXT ('Student' | 'Admin')"
        string specific_id "TEXT (STU1001 / ADM001)"
        string created_at "TEXT (ISO8601)"
    }

    QUIZZES {
        int id PK "INTEGER AUTOINCREMENT"
        string title "TEXT"
        string category "TEXT"
        string description "TEXT"
        int duration_minutes "INTEGER"
        real pass_percentage "REAL"
        int total_marks "INTEGER"
        string created_at "TEXT (ISO8601)"
    }

    QUESTIONS {
        int id PK "INTEGER AUTOINCREMENT"
        int quiz_id FK "INTEGER NOT NULL"
        string question_text "TEXT NOT NULL"
        string option_a "TEXT NOT NULL"
        string option_b "TEXT NOT NULL"
        string option_c "TEXT NOT NULL"
        string option_d "TEXT NOT NULL"
        int correct_option "INTEGER (0 to 3)"
        int marks "INTEGER DEFAULT 1"
        string explanation "TEXT"
    }

    RESULTS {
        int id PK "INTEGER AUTOINCREMENT"
        int student_id FK "INTEGER NOT NULL"
        int quiz_id FK "INTEGER NOT NULL"
        int total_questions "INTEGER"
        int correct_count "INTEGER"
        int incorrect_count "INTEGER"
        int unanswered_count "INTEGER"
        int total_marks "INTEGER"
        int marks_obtained "INTEGER"
        real percentage "REAL"
        int is_passed "INTEGER (0 or 1)"
        string attempt_date "TEXT"
        int time_taken_seconds "INTEGER"
    }

    USERS ||--o{ RESULTS : "attempts (student_id)"
    QUIZZES ||--|{ QUESTIONS : "contains (quiz_id)"
    QUIZZES ||--o{ RESULTS : "evaluated in (quiz_id)"
```
