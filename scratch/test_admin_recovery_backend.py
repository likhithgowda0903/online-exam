import sqlite3
import json

print("=== TESTING ADMINISTRATOR RECOVERY API & SQLITE SYNC ===")

# Check database
conn = sqlite3.connect('online_exam.db')
c = conn.cursor()

# Insert a test admin into users table
c.execute("""
    INSERT OR REPLACE INTO users (username, password, role, specific_id, full_name, email, created_at)
    VALUES ('test_admin_faculty', 'OldAdminPass123', 'admin', 'ADM555', 'Prof. Faculty Admin', 'prof@faculty.edu', '2026-11-15 16:00:00')
""")
conn.commit()

# Test 1: Verify query logic
c.execute("""
    SELECT id, username, specific_id, full_name, email 
    FROM users 
    WHERE role = 'admin' 
      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
      AND LOWER(email) = ?
""", ('adm555', 'adm555', 'prof@faculty.edu'))
row = c.fetchone()
assert row is not None, "Failed to find administrator in SQLite"
print("✓ SQLite verification query matched administrator account:", row[1], row[2], row[3])

# Test 2: Mismatched email rejected
c.execute("""
    SELECT id, username, specific_id, full_name, email 
    FROM users 
    WHERE role = 'admin' 
      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
      AND LOWER(email) = ?
""", ('adm555', 'adm555', 'wrong@faculty.edu'))
row = c.fetchone()
assert row is None, "Mismatched email must not match any admin"
print("✓ SQLite verification query properly rejected invalid email")

# Test 3: Password reset update in SQLite
c.execute("""
    UPDATE users 
    SET password = ? 
    WHERE role = 'admin' 
      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
""", ('BrandNewSecuredPass2026!', 'adm555', 'adm555'))
conn.commit()

# Verify updated password
c.execute("SELECT password FROM users WHERE username = 'test_admin_faculty'")
updated_pass = c.fetchone()[0]
assert updated_pass == 'BrandNewSecuredPass2026!', "Password was not updated in SQLite"
print("✓ SQLite administrator password updated successfully to:", updated_pass)

# Clean up test admin
c.execute("DELETE FROM users WHERE username = 'test_admin_faculty'")
conn.commit()
conn.close()

print("\n✓ All SQLite 3 administrator verification and password reset logic verified!")
