# 🎓 Online Examination System (C++ OOPs & Qt 6)

A complete, modern, and professional **Online Examination System** mini project built using **C++17 Object-Oriented Programming (OOPs)**, **Qt 6 GUI**, and **SQLite** for persistent storage.

Designed and structured specifically for academic submissions (such as **MCA, B.Tech, or BCA Mini Projects**), featuring clean architectural separation, design patterns, password cryptography, automatic evaluation, and a modern responsive dashboard UI.

---

## 🌟 Key Features & Modules

### 1. Student Registration & Validation
* Register with Full Name, Student ID (e.g., `STU1001`), Email, Username, Password, and Confirm Password.
* Client-side validation: Regular expression email format check, minimum 6-character password constraint, matching password verification.
* Server/Database validation: Duplicate account prevention (unique username, email, and student ID).

### 2. Secure Role-Based Authentication
* Dual-role access control for **Students** and **Administrators**.
* Cryptographic password security: Passwords are **never stored in plain text**. Uses **SHA-256** combined with unique 16-character cryptographic salting (`QCryptographicHash`).
* Pre-seeded default credentials for rapid evaluation:
  * **Administrator:** Username: `admin` | Password: `Admin@123`
  * **Student:** Username: `student1` | Password: `Student@123`

### 3. Student Dashboard
* Personalized welcome banner displaying student's full name, ID, and email.
* Real-time academic statistics cards: Total Exams Taken, Average Score Percentage, and Passed Examinations.
* **Available Examinations Table**: Lists all active exams with Title, Category, Questions Count, Total Marks, Duration, and a direct `Start Exam ▶` action.
* **Examination History Table**: Lists all historical attempts with date, score %, marks obtained, status badge (`PASSED` / `FAILED`), and time taken, with a `Scorecard 📋` review button.

### 4. Admin Dashboard & Analytics
* High-level institutional metrics: Registered Students, Available Quizzes, Total Exams Completed, and Global Average Score.
* **Manage Examinations Tab**: Create, edit, and delete quizzes; manage question pools, configure time limits and pass percentages.
* **Manage Students Tab**: View all registered students, their exam counts, average scores, and manage enrollment.
* **Results & Analytics Tab**: Comprehensive record of every student attempt across the institution with drill-down scorecards.

### 5. Quiz Management (Admin Builder)
* Dynamic Quiz Builder allowing administrators to specify Title, Category, Duration (minutes), Pass Percentage (%), and Description.
* Add/Edit/Delete individual Multiple-Choice Questions (MCQs):
  * Question prompt.
  * 4 selectable choices (Option A, B, C, D).
  * Designated correct answer.
  * Assigned marks.
  * Detailed pedagogical explanation for answer review.
* Automatic recalculation of total exam marks upon question modifications.

### 6. Interactive Online Examination Window
* Focused single-question display card preventing cognitive overload.
* 4 distinct radio button choices with modern visual feedback.
* Clear question navigation: `Previous`, `Next`, and `Clear Selection`.
* `🚩 Mark for Review` flag to allow students to flag questions and return to them later.
* **Question Palette**: Color-coded question status grid:
  * 🟩 **Green**: Answered
  * ⬜ **Gray**: Unanswered
  * 🟧 **Orange**: Marked for Review
  * 🟦 **Cyan Border**: Current Active Question
* Instant jumping to any question by clicking its palette badge.

### 7. Countdown Timer Engine
* Real-time countdown clock in `MM:SS` format updating every second.
* Automatic warning dialog and visual alert when under 2 minutes remaining.
* **Automatic submission** when the timer expires to maintain examination integrity.

### 8. Real-Time AI Webcam Proctoring & Anti-Cheating Detection Engine
* **Automatic Webcam Activation**: Instantly activates candidate's webcam as soon as an examination starts.
* **Continuous Facial Presence & Centering**: Computer vision algorithm tracks facial chromaticity and centroid coordinates in real-time. Detects if candidate leaves camera frame or covers the camera lens.
* **Looking-Away & Eye Deviation Detection**: Flags when candidate looks away from the exam window or turns their head toward suspicious angles for > 2 seconds.
* **Multiple Faces Detection**: Identifies whether secondary individuals enter the camera field of view.
* **Tab-Switch & Window Blur Monitoring**: Detects when candidates minimize the exam window or open external browser tabs to search for answers.
* **Keyboard Shortcut Blocker**: Blocks `F12` Developer Tools, `Ctrl+C`, `Ctrl+V`, and copy-paste attempts.
* **Audible & Visual Strike Warning System**: Alerts candidates with audio beeps and progressive strike penalties (`0 / 4 Strikes`).
* **Integrity Audit Report in Final Scorecard**: Certified trust percentage score (e.g. `100% Verified` vs `Flagged`), detailed incident audit log with timestamps, and supervisor review recommendations.

### 9. Polymorphic Automatic Evaluation & Score Reports
* Compares student selections against encrypted answer keys upon submission.
* Computes: Total Questions, Correct Answers, Incorrect Answers, Unanswered Count, Total Marks, Marks Obtained, and Percentage Score.
* Determines Pass/Fail status based on the quiz's pass threshold.
* Generates an official scorecard with detailed question-by-question breakdown, showing student selection vs correct answer and the rationale/explanation.

### 9. SQLite Persistent Database
* File-based SQLite database (`online_exam.db`) created automatically on first run.
* Foreign keys enabled with cascading deletions.
* Automatically seeds default administrator, demo student, and 3 rich pre-built exams (C++ OOP Mastery, Data Structures, DBMS & SQL).

---

## 🏗️ Object-Oriented Programming (OOP) Implementation

| OOP Concept | Project Implementation |
|---|---|
| **Encapsulation** | All models (`User`, `Student`, `Admin`, `Question`, `Quiz`, `Result`, `ExamSession`) maintain `private` member variables with strict public getters and validation setters. Passwords and answer keys are shielded from unauthorized external modification. |
| **Inheritance** | Base class `User` is extended by `Student` (adding student ID, exams taken count, and GPA) and `Admin` (adding staff code and department). |
| **Polymorphism** | Pure virtual function `virtual QString getRole() const = 0` in `User` is overridden by `Student` and `Admin`. The UI polymorphic dispatcher (`MainWindow::loadUserDashboard`) dynamically instantiates the correct portal at runtime via `std::dynamic_pointer_cast`. |
| **Abstraction** | Pure abstract interfaces such as `IEvaluationStrategy` define scoring contracts, allowing different evaluation algorithms (standard vs negative marking) to be swapped cleanly. |
| **Design Patterns** | **Singleton Pattern** in `DatabaseManager`, **Strategy Pattern** in `IEvaluationStrategy`, and **Observer/Event-driven Pattern** via Qt Signals & Slots in `ExamTimer`. |

---

## 📁 Project Directory Structure

```text
online-exam/
├── START_WEBSITE.bat               # 🚀 One-Click Launcher: Starts server & opens website in browser
├── run_app.bat                     # Unified Application Launcher
├── start_server.py                 # Automated multi-port local web server engine
├── index.html                      # Full Single-Page Web Application Portal
├── README.md                       # Complete documentation & user guide
├── PROJECT_REPORT.md               # MCA Mini Project formal academic report
├── docs/
│   └── UML_DIAGRAMS.md             # UML Architecture, Class & Sequence Diagrams
├── web-portal/                     # 🌐 Standalone Web Portal Folder
│   ├── START_WEBSITE.bat           # Web launcher inside portal folder
│   ├── start_server.py             # Web server engine
│   └── index.html                  # Web Application Portal (HTML5, CSS3, JavaScript)
└── desktop-app/                    # 🖥️ C++ Qt6 Desktop Application Folder
    ├── build.bat                   # CMake compilation script for C++
    ├── run_desktop.bat             # Desktop application launcher
    ├── CMakeLists.txt              # Modern CMake build configuration
    ├── OnlineExamSystem.pro        # Qt Creator project file
    ├── resources.qrc               # Qt resource bundle
    ├── styles/
    │   └── modern_theme.qss        # Professional dark QSS stylesheet
    ├── tests/
    │   └── test_exam_system.cpp    # Unit testing suite
    ├── include/                    # C++ Header files (models, core, database, ui)
    └── src/                        # C++ Implementation source files
---

## 🚀 How to Run

### 🌐 Method 1: Running the Web Application (Easiest & Instant)
Double-click **`START_WEBSITE.bat`** in the project root folder.
* Automatically initializes the local web server on port `5500` (or `8000`).
* Opens your default web browser directly to the portal.
* Enables full hardware webcam AI proctoring, exam timer, student candidate dashboard, and administrator console.
* Alternatively, you can open `index.html` directly in any modern web browser.

---

### 🖥️ Method 2: Running the C++ Qt6 Desktop Application

### Prerequisites
* **C++ Compiler**: GCC (MinGW 8+), Clang (10+), or MSVC (2019/2022) with C++17 support.
* **Qt Framework**: Qt 6.x (recommended) or Qt 5.15 with components `Widgets`, `Gui`, `Core`, and `Sql`.
* **CMake**: Version 3.16 or higher (or `qmake` via Qt Creator).

---

### Option 1: Using Qt Creator (Easiest)
1. Launch **Qt Creator**.
2. Select **File > Open File or Project...** and choose `OnlineExamSystem.pro` (or `CMakeLists.txt`).
3. Select your installed Qt Kit (e.g., `Desktop Qt 6.x.x MinGW 64-bit` or `MSVC 64-bit`).
4. Click the green **Run** button (or press `Ctrl + R`).
5. The application will compile, generate `online_exam.db`, and display the Login dialog!

---

### Option 2: Using CMake on the Command Line

#### On Windows (PowerShell or Command Prompt):
```powershell
# Navigate to the project directory
cd "online exam"

# Create a build directory
mkdir build
cd build

# Configure with CMake (adjust generator if needed)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compile the project
cmake --build . --config Release

# Run the application
./OnlineExamSystem
```

#### On Linux / macOS:
```bash
cd "online exam"
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
./OnlineExamSystem
```

---

### Running the Unit Test Suite

To verify OOP polymorphism, hashing, and evaluation logic without the GUI:
```powershell
cd build
cmake --build . --target test_exam_system
./test_exam_system
```

Expected Output:
```text
=================================================
Running Online Examination System OOP Unit Tests
=================================================
[TEST] 1. Testing Polymorphism & Inheritance...
  ✓ Polymorphism and Inheritance verified successfully.
[TEST] 2. Testing Password Hashing & Salt Verification...
  ✓ Cryptographic SHA-256 password security verified successfully.
[TEST] 3. Testing Question Encapsulation & Quiz Aggregation...
  ✓ Question and Quiz aggregation verified successfully.
[TEST] 4. Testing Scoring Engine & ExamSession...
  ✓ Automatic evaluation engine verified successfully.
=================================================
ALL 4 UNIT TEST SUITES PASSED SUCCESSFULLY (100%)
=================================================
```

---

## ⚡ Instant Browser Preview & Demonstration

If Qt 6 is not yet installed on your machine and you want to demonstrate the interface, interactive question flow, timer, and scorecards immediately:

1. Open `demo_preview.html` in any web browser (Chrome, Edge, Firefox).
2. Use the quick buttons to log in as **Admin** or **Student**.
3. Attempt the pre-loaded quizzes, test the countdown timer, mark questions for review, and submit to see the scorecard!
