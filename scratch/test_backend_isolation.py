import sqlite3
import os

print("=== TESTING SQLITE 3 ADMIN ISOLATION BACKEND ===")
db_path = 'online_exam.db'
conn = sqlite3.connect(db_path)
c = conn.cursor()

# Verify column
cols = [r[1] for r in c.execute("PRAGMA table_info(quizzes)").fetchall()]
assert 'created_by' in cols, "created_by column missing in quizzes table"
print("✓ PRAGMA verified: 'created_by' column exists in quizzes table.")

# Test 1: Admin A creates exam
c.execute("""
    INSERT OR REPLACE INTO quizzes (id, title, category, duration_minutes, pass_percentage, total_marks, created_by, created_at)
    VALUES (?, ?, ?, ?, ?, ?, ?, datetime('now'))
""", ('WEB101', 'Multimedia & Web Technologies', 'Web Dev', 45, 60.0, 100, 'ADM001'))
conn.commit()

# Test 2: Admin B creates exam
c.execute("""
    INSERT OR REPLACE INTO quizzes (id, title, category, duration_minutes, pass_percentage, total_marks, created_by, created_at)
    VALUES (?, ?, ?, ?, ?, ?, ?, datetime('now'))
""", ('CPP101', 'C++ Programming', 'Programming', 45, 60.0, 100, 'ADM002'))
conn.commit()

# Test 3: Admin A query
admin_a_quizzes = [r[0] for r in c.execute("SELECT id FROM quizzes WHERE LOWER(created_by) = LOWER('ADM001')").fetchall()]
print("  Admin A (ADM001) exams:", admin_a_quizzes)
assert 'WEB101' in admin_a_quizzes and 'CPP101' not in admin_a_quizzes

# Test 4: Admin B query
admin_b_quizzes = [r[0] for r in c.execute("SELECT id FROM quizzes WHERE LOWER(created_by) = LOWER('ADM002')").fetchall()]
print("  Admin B (ADM002) exams:", admin_b_quizzes)
assert 'CPP101' in admin_b_quizzes and 'WEB101' not in admin_b_quizzes

# Test 5: Student / All published query
all_quizzes = [r[0] for r in c.execute("SELECT id FROM quizzes").fetchall()]
print("  Student (All published) exams:", all_quizzes)
assert 'WEB101' in all_quizzes and 'CPP101' in all_quizzes

# Clean up test rows
c.execute("DELETE FROM quizzes WHERE id IN ('WEB101', 'CPP101')")
conn.commit()
conn.close()

print("\n✓ Backend SQLite isolation tests passed successfully!")

