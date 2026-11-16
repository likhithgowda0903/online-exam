import sqlite3

conn = sqlite3.connect('online_exam.db')
c = conn.cursor()

# 1. Insert a test registered student into users table
c.execute("""
    INSERT OR REPLACE INTO users (username, password, full_name, email, role, specific_id, created_at)
    VALUES ('test_candidate', 'InitialSecret123', 'Test Candidate', 'test@student.edu', 'student', 'STU9901', datetime('now'))
""")
conn.commit()

# 2. Test Verification query
c.execute("""
    SELECT id, username, specific_id, full_name, email 
    FROM users 
    WHERE role = 'student' 
      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
      AND LOWER(email) = ?
""", ('stu9901', 'stu9901', 'test@student.edu'))
row = c.fetchone()
assert row is not None, "Failed to verify student with roll number and email!"
print("✓ SQLite Verification query successful:", row)

# 3. Test Reset query
c.execute("""
    UPDATE users 
    SET password = ? 
    WHERE role = 'student' 
      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
""", ('UpdatedPassword456', 'stu9901', 'stu9901'))
conn.commit()
print("✓ SQLite Password Update query rows affected:", c.rowcount)

# 4. Verify new password
c.execute("SELECT password FROM users WHERE username = 'test_candidate'")
new_p = c.fetchone()[0]
assert new_p == 'UpdatedPassword456', "Password was not updated in SQLite!"
print("✓ Verified new password in SQLite users table:", new_p)

# 5. Clean up test row
c.execute("DELETE FROM users WHERE username = 'test_candidate'")
conn.commit()
conn.close()

print("\n>>> SQLITE RECOVERY BACKEND INTEGRATION TEST PASSED! <<<")

