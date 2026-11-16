# 🎓 Mini Project Report: Online Examination System

**Degree Program:** Master of Computer Applications (MCA) / Bachelor of Computer Applications (BCA) / B.Tech  
**Course:** Object-Oriented Software Engineering & Advanced C++ Programming  
**Project Title:** Online Examination System  
**Technology Stack:** C++17, Qt 6 (Widgets/Sql/Gui/Core), SQLite3  

---

## 1. Abstract

The **Online Examination System** is a software application designed to automate, streamline, and secure the process of conducting academic assessments. Developed using **C++17 Object-Oriented Programming (OOPs)**, the **Qt 6 Framework**, and an embedded **SQLite** database, the application provides a modern, responsive, and robust desktop environment for both educators (Administrators) and learners (Students). 

The system implements role-based access control, cryptographic password hashing via salted SHA-256, timed online tests with a countdown timer, instant automated evaluation, detailed graphical scorecards, and institutional performance tracking. By applying core OOP principles—**Encapsulation, Inheritance, Polymorphism, and Abstraction**—alongside recognized software design patterns (**Singleton, Strategy, and Observer**), the codebase achieves high modularity, maintainability, and testability.

---

## 2. Problem Statement

Traditional paper-based examinations suffer from significant administrative overhead, high paper and printing costs, logistical hurdles, delays in manual answer sheet evaluation, and risks of human grading errors. Furthermore, basic computerized testing systems often store plaintext credentials, lack question-palette navigation, fail to automatically enforce countdown expiration, and lack modular architecture.

### Objectives of the Proposed System:
1. **Security:** Implement cryptographic password hashing with salting to guarantee that student and administrative credentials are never stored in plaintext.
2. **Pedagogical Assessment:** Provide single-question focused navigation, a color-coded question palette, and mark-for-review capabilities.
3. **Timer Discipline:** Enforce strict examination windows with an automatic warning when time runs low, and automatic submission upon expiration.
4. **Immediate Feedback:** Eliminate manual grading delay by comparing submissions against stored answer keys and displaying a comprehensive scorecard.
5. **Architectural Rigor:** Demonstrate core C++ OOP concepts and design patterns suitable for higher-degree academic evaluation.

---

## 3. Software Requirements Specification (SRS)

### 3.1 Software Requirements
* **Operating System:** Windows 10/11, Ubuntu 20.04+, or macOS 12+
* **Programming Language:** C++ (Standard ISO C++17 or later)
* **GUI Toolkit:** Qt 6.2+ / Qt 5.15 (Widgets, Core, Gui, Sql)
* **Database Engine:** SQLite 3 (accessed via `QSqlDatabase`)
* **Build System:** CMake 3.16+ or qmake / Qt Creator IDE
* **Cryptography:** Qt Cryptographic Library (`QCryptographicHash`)

### 3.2 Hardware Requirements
* **Processor:** 1.5 GHz Dual Core x86_64 or ARM64 processor
* **RAM:** Minimum 2 GB (4 GB recommended)
* **Storage:** 100 MB available hard disk space

---

## 4. Object-Oriented Programming (OOP) Implementation

The system was architected from the ground up to demonstrate mastery of the four pillars of OOP:

### 4.1 Abstraction
* **Abstract Base Class `User`:** Declares the pure virtual function `virtual QString getRole() const = 0`, defining a clean identity interface without dictating role-specific implementation.
* **Evaluation Strategy `IEvaluationStrategy`:** An abstract interface declaring `virtual Result evaluate(const ExamSession& session) = 0`. This decouples the scoring logic from the UI and active exam session, permitting alternative grading schemes (e.g., negative marking or percentile scaling) without modifying the exam window.

### 4.2 Encapsulation
* All domain models (`User`, `Student`, `Admin`, `Question`, `Quiz`, `Result`, `ExamSession`) store their state in `private` or `protected` member variables.
* State modifications are mediated through public member functions containing validation checks (e.g., preventing negative marks, ensuring four non-empty question options, sanitizing string inputs).
* Database queries use parameterized prepared statements (`QSqlQuery::prepare` and `bindValue`), encapsulating SQL syntax and immunizing the application against SQL injection attacks.

### 4.3 Inheritance
* A clean inheritance hierarchy links the base identity class `User` to specialized derived classes:
  * `class Student : public User` adds student-specific attributes (`m_studentId`, `m_totalExamsTaken`, `m_averageScore`).
  * `class Admin : public User` adds institutional attributes (`m_adminCode`, `m_department`).

### 4.4 Polymorphism
* **Run-Time Dynamic Dispatch:** Virtual function overrides (`getRole()`, `getSpecificId()`) allow base class pointers (`std::shared_ptr<User>`) to dynamically invoke derived behaviors.
* **Polymorphic UI Dispatch:** In `MainWindow::loadUserDashboard()`, the application queries the polymorphic `User::getRole()` method and uses `std::dynamic_pointer_cast` to launch either the `StudentDashboard` or `AdminDashboard` without hardcoding conditional branching across multiple forms.

---

## 5. Software Design Patterns

1. **Singleton Pattern (`DatabaseManager`):** Guarantees a single centralized instance managing SQLite database connections, statement execution, and table migrations throughout the application lifecycle.
2. **Strategy Pattern (`IEvaluationStrategy` / `StandardEvaluator`):** Encapsulates the algorithm for calculating student marks and percentages behind an interchangeable interface.
3. **Observer / Event-Driven Pattern (`ExamTimer`):** Utilizes Qt's Signals and Slots architecture. The timer emits `tick()`, `warningLowTime()`, and `timeout()` signals, and the `ExamWindow` automatically reacts to update progress and trigger auto-submission.
4. **Dynamic Randomization & Rotation Strategy (`IQuestionSelector` / `RandomizedQuestionSelector` / `QuestionBank`):** Uses modern C++17 `<random>` (`std::mt19937_64` and `std::shuffle`) to dynamically draw a fresh, non-repeating subset of questions and randomly permute choice options (A, B, C, D) after every exam submission, guaranteeing that subsequent attempts present new questions from the master question pool.

---

## 6. Database Schema Design

The SQLite database (`online_exam.db`) consists of four relational tables:

### 6.1 Table: `users`
| Column | Data Type | Constraints | Description |
|---|---|---|---|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique user identifier |
| `username` | TEXT | UNIQUE, NOT NULL | Account login username |
| `password_hash` | TEXT | NOT NULL | Salted SHA-256 password hash |
| `salt` | TEXT | NOT NULL | 16-character cryptographic salt |
| `full_name` | TEXT | NOT NULL | Full name of the individual |
| `email` | TEXT | UNIQUE, NOT NULL | Unique email address |
| `role` | TEXT | NOT NULL | 'Student' or 'Admin' |
| `specific_id` | TEXT | NOT NULL | Student ID or Admin staff code |
| `created_at` | TEXT | NOT NULL | Timestamp of account registration |

### 6.2 Table: `quizzes`
| Column | Data Type | Constraints | Description |
|---|---|---|---|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique quiz identifier |
| `title` | TEXT | NOT NULL | Title of the examination |
| `category` | TEXT | NOT NULL | Subject area / category |
| `description` | TEXT | - | Instructions and overview |
| `duration_minutes` | INTEGER | NOT NULL | Duration in minutes |
| `pass_percentage` | REAL | NOT NULL | Minimum score required to pass |
| `total_marks` | INTEGER | NOT NULL DEFAULT 0 | Cumulative marks of all questions |
| `created_at` | TEXT | NOT NULL | Quiz creation timestamp |

### 6.3 Table: `questions`
| Column | Data Type | Constraints | Description |
|---|---|---|---|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique question identifier |
| `quiz_id` | INTEGER | FOREIGN KEY REFERENCES quizzes(id) | Parent quiz association |
| `question_text` | TEXT | NOT NULL | The MCQ prompt |
| `option_a` | TEXT | NOT NULL | Choice A |
| `option_b` | TEXT | NOT NULL | Choice B |
| `option_c` | TEXT | NOT NULL | Choice C |
| `option_d` | TEXT | NOT NULL | Choice D |
| `correct_option` | INTEGER | NOT NULL (0-3) | Index of the correct answer |
| `marks` | INTEGER | NOT NULL DEFAULT 1 | Marks awarded for correct answer |
| `explanation` | TEXT | - | Pedagogical explanation |

### 6.4 Table: `results`
| Column | Data Type | Constraints | Description |
|---|---|---|---|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique result scorecard ID |
| `student_id` | INTEGER | FOREIGN KEY REFERENCES users(id) | Attempting student ID |
| `quiz_id` | INTEGER | FOREIGN KEY REFERENCES quizzes(id) | Attempted quiz ID |
| `total_questions` | INTEGER | NOT NULL | Total questions presented |
| `correct_count` | INTEGER | NOT NULL | Number of correct selections |
| `incorrect_count` | INTEGER | NOT NULL | Number of incorrect selections |
| `unanswered_count`| INTEGER | NOT NULL | Number of unanswered questions |
| `total_marks` | INTEGER | NOT NULL | Maximum possible marks |
| `marks_obtained` | INTEGER | NOT NULL | Marks scored by the candidate |
| `percentage` | REAL | NOT NULL | Percentage scored |
| `is_passed` | INTEGER | NOT NULL (0 or 1) | 1 = Passed, 0 = Failed |
| `attempt_date` | TEXT | NOT NULL | Formatted examination timestamp |
| `time_taken_seconds` | INTEGER | NOT NULL | Total elapsed duration in seconds |

---

## 7. Test Cases & Verification Results

| Test ID | Module | Scenario / Input | Expected Result | Actual Result | Status |
|---|---|---|---|---|---|
| **TC-01** | Registration | Duplicate username registration | Error message: "Username is already registered." | Error displayed; registration rejected | **PASS** |
| **TC-02** | Registration | Invalid email format (`user@domain`) | Regex validation error: "Please enter a valid email address." | Error displayed; form blocked | **PASS** |
| **TC-03** | Login | Incorrect password for existing user | Error message: "Incorrect password. Please try again." | Login rejected; error shown | **PASS** |
| **TC-04** | Authentication | Password storage check in SQLite | Passwords stored as 64-character SHA-256 hexadecimal hash with salt | Verified in `online_exam.db`; zero plaintext passwords | **PASS** |
| **TC-05** | Online Exam | Option selection and palette update | Palette button turns green and progress bar advances | Button style updates to `#paletteAnswered` | **PASS** |
| **TC-06** | Online Exam | "Mark for Review" toggling | Palette button turns orange; button text updates | Button style updates to `#paletteReview` | **PASS** |
| **TC-07** | Countdown Timer | Low time warning (< 2 mins) | Visual warning dialog and timer border turns red | Warning dialog appears; border turns `#ef4444` | **PASS** |
| **TC-08** | Countdown Timer | Timer reaches 00:00 | Automatic submission without candidate intervention | Exam auto-submits; scorecard opens | **PASS** |
| **TC-09** | Evaluation | Score computation (e.g., 4/5 correct) | Marks = 20/25, Percentage = 80.0%, Status = PASSED | Exact values computed and saved | **PASS** |
| **TC-10** | Admin Portal | Create new quiz with questions | New quiz appears in Student Dashboard immediately | Verified persistent in database and UI | **PASS** |

---

## 8. Conclusion & Future Enhancements

The **Online Examination System** successfully addresses the operational inefficiencies and security risks inherent in manual paper exams. By applying C++17 object-oriented architecture alongside Qt 6 desktop components, the project delivers a production-grade, aesthetically pleasing, and technically rigorous mini project.

### Future Enhancements:
1. **Networked Client-Server Architecture:** Extending SQLite to a centralized PostgreSQL/MySQL database server.
2. **Automated Question Shuffling:** Randomizing question order and options per candidate to mitigate academic dishonesty.
3. **Webcam Proctoring:** Integrating OpenCV to capture periodic candidate snapshots during the examination.
4. **PDF Scorecard Export:** Generating digitally signed PDF certificates of completion using Qt's `QPrinter` / `QPdfWriter`.
