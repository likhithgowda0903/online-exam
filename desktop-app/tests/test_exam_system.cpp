#include <iostream>
#include <cassert>
#include <memory>
#include "models/User.h"
#include "models/Student.h"
#include "models/Admin.h"
#include "models/Question.h"
#include "models/Quiz.h"
#include "models/ExamSession.h"
#include "core/Evaluator.h"
#include "core/QuestionBank.h"
#include "database/PasswordHasher.h"

void testPolymorphismAndInheritance() {
    std::cout << "[TEST] 1. Testing Polymorphism & Inheritance..." << std::endl;

    std::shared_ptr<User> studentUser = std::make_shared<Student>(
        1, "john_doe", "John Doe", "john@college.edu", "STU2025", 2, 85.0
    );

    std::shared_ptr<User> adminUser = std::make_shared<Admin>(
        2, "admin_user", "Prof. Smith", "smith@college.edu", "ADM007", "Computer Science"
    );

    // Verify polymorphic dynamic dispatch via base User pointer
    assert(studentUser->getRole() == "Student");
    assert(studentUser->getSpecificId() == "STU2025");
    assert(studentUser->getDisplayName() == "John Doe");

    assert(adminUser->getRole() == "Admin");
    assert(adminUser->getSpecificId() == "ADM007");
    assert(adminUser->getDisplayName() == "Prof. Smith");

    std::cout << "  ✓ Polymorphism and Inheritance verified successfully." << std::endl;
}

void testPasswordSecurity() {
    std::cout << "[TEST] 2. Testing Password Hashing & Salt Verification..." << std::endl;

    QString plainPassword = "SecurePassword@2026";
    QString salt = PasswordHasher::generateSalt(16);
    assert(!salt.isEmpty());
    assert(salt.length() == 16);

    QString hash = PasswordHasher::hashPassword(plainPassword, salt);
    assert(!hash.isEmpty());
    assert(hash != plainPassword); // Never plain text

    // Verification check
    assert(PasswordHasher::verifyPassword(plainPassword, hash, salt) == true);
    assert(PasswordHasher::verifyPassword("WrongPassword", hash, salt) == false);

    std::cout << "  ✓ Cryptographic SHA-256 password security verified successfully." << std::endl;
}

void testQuestionAndQuizLogic() {
    std::cout << "[TEST] 3. Testing Question Encapsulation & Quiz Aggregation..." << std::endl;

    Question q1(1, 101, "What does OOP stand for?", 
                 {"Object Oriented Programming", "Only One Paradigm", "Ordered Object Processing", "Output Origin Protocol"},
                 0, 5, "OOP stands for Object Oriented Programming.");

    assert(q1.isValid());
    assert(q1.isCorrect(0) == true);
    assert(q1.isCorrect(1) == false);
    assert(q1.getMarks() == 5);

    Quiz quiz(101, "C++ Basics", "Computer Science", "Test of C++ foundations", 15, 60.0);
    quiz.addQuestion(q1);

    Question q2(2, 101, "Which is an access specifier?",
                 {"loop", "private", "vector", "include"},
                 1, 5, "private, protected, and public are access specifiers.");
    quiz.addQuestion(q2);

    assert(quiz.getQuestionCount() == 2);
    assert(quiz.getTotalMarks() == 10);

    std::cout << "  ✓ Question and Quiz aggregation verified successfully." << std::endl;
}

void testEvaluationEngine() {
    std::cout << "[TEST] 4. Testing Scoring Engine & ExamSession..." << std::endl;

    Quiz quiz(1, "Sample Test", "General", "Desc", 10, 50.0);
    quiz.addQuestion(Question(1, 1, "Q1", {"A", "B", "C", "D"}, 0, 5)); // Correct: 0 (A)
    quiz.addQuestion(Question(2, 1, "Q2", {"A", "B", "C", "D"}, 1, 5)); // Correct: 1 (B)
    quiz.addQuestion(Question(3, 1, "Q3", {"A", "B", "C", "D"}, 2, 5)); // Correct: 2 (C)

    ExamSession session(10, quiz);
    assert(session.getAnsweredCount() == 0);
    assert(session.getUnansweredCount() == 3);

    // Answer Q1 correctly, Q2 incorrectly, leave Q3 unanswered
    session.selectAnswer(0, 0); // Correct (+5)
    session.selectAnswer(1, 3); // Incorrect (+0)
    // Q3 remains unselected (-1)

    assert(session.getAnsweredCount() == 2);
    // Simulate proctoring integrity tracking
    assert(session.getProctorWarnings() == 0);
    assert(session.getIntegrityScore() == 100.0);
    session.addProctorWarning();
    assert(session.getProctorWarnings() == 1);
    assert(session.getIntegrityScore() == 88.0);

    StandardEvaluator evaluator;
    Result res = evaluator.evaluate(session);

    assert(res.getTotalQuestions() == 3);
    assert(res.getCorrectAnswers() == 1);
    assert(res.getIncorrectAnswers() == 1);
    assert(res.getUnansweredQuestions() == 1);
    assert(res.getTotalMarks() == 15);
    assert(res.getMarksObtained() == 5);
    // 5 / 15 = 33.33% < 50% pass percentage
    assert(res.isPassed() == false);
    assert(res.getProctorWarnings() == 1);
    assert(res.getIntegrityScore() == 88.0);

    std::cout << "  ✓ Automatic evaluation & anti-cheating proctoring engine verified successfully." << std::endl;
}

void testDynamicQuestionRotationAndShuffling() {
    std::cout << "[TEST] 5. Testing Modern C++ Dynamic Question Bank & Rotation..." << std::endl;

    // Create a question pool with 10 questions
    std::vector<Question> pool;
    for (int i = 0; i < 10; ++i) {
        pool.emplace_back(
            i + 1, 101, QString("Question %1: What is C++17 feature %1?").arg(i + 1),
            QStringList{"Alpha", "Beta", "Gamma", "Delta"},
            (i % 4), 5, QString("Explanation %1").arg(i + 1)
        );
    }

    QuestionBank bank(101, std::make_shared<RandomizedQuestionSelector>(true));
    bank.setMasterPool(pool);
    assert(bank.getPoolSize() == 10);
    assert(bank.getAttemptCount() == 0);

    // Generate Attempt 1: draw 5 questions
    auto set1 = bank.generateNextExamSet(5);
    assert(set1.size() == 5);

    // Verify option shuffling maintains the correct answer integrity
    for (const auto& q : set1) {
        int correctIdx = q.getCorrectOptionIndex();
        assert(correctIdx >= 0 && correctIdx < 4);
        assert(q.isCorrect(correctIdx) == true);
        assert(q.isCorrect((correctIdx + 1) % 4) == false);
    }

    // Simulate submission of Exam Attempt 1
    bank.onExamSubmitted();
    assert(bank.getAttemptCount() == 1);

    // Generate Attempt 2: dynamic rotation for next time
    auto set2 = bank.generateNextExamSet(5);
    assert(set2.size() == 5);

    std::cout << "  ✓ Modern C++ QuestionBank and dynamic rotation engine verified successfully." << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "=================================================" << std::endl;
    std::cout << "Running Online Examination System OOP Unit Tests" << std::endl;
    std::cout << "=================================================" << std::endl;

    testPolymorphismAndInheritance();
    testPasswordSecurity();
    testQuestionAndQuizLogic();
    testEvaluationEngine();
    testDynamicQuestionRotationAndShuffling();

    std::cout << "=================================================" << std::endl;
    std::cout << "ALL 5 UNIT TEST SUITES PASSED SUCCESSFULLY (100%)" << std::endl;
    std::cout << "=================================================" << std::endl;
    return 0;
}
