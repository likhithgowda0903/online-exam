import sqlite3
import os
import re

print("=== 1. CLEANING SQLITE DATABASES ===")
db_paths = ['online_exam.db', 'web-portal/online_exam.db', 'desktop-app/online_exam.db']
for db_path in db_paths:
    if not os.path.exists(db_path):
        continue
    conn = sqlite3.connect(db_path)
    c = conn.cursor()
    c.execute("DELETE FROM users WHERE username IN ('student', 'admin');")
    conn.commit()
    c.execute("SELECT count(*) FROM users;")
    count = c.fetchone()[0]
    print(f"  {db_path}: Remaining users in users table = {count}")
    conn.close()

print("\n=== 2. UPDATING start_server.py & web-portal/start_server.py ===")
server_files = ['start_server.py', 'web-portal/start_server.py']
seed_code_block = """    # Seed default credentials in SQLite
    c.execute('''
        INSERT OR IGNORE INTO users (username, password, full_name, email, role, specific_id, created_at)
        VALUES ('student', 'password', 'Student Candidate', 'student@examportal.edu', 'student', 'STU1001', datetime('now'))
    ''')
    c.execute('''
        INSERT OR IGNORE INTO users (username, password, full_name, email, role, specific_id, created_at)
        VALUES ('admin', 'admin', 'System Administrator', 'admin@examportal.edu', 'admin', 'ADM101', datetime('now'))
    ''')"""

clean_seed_block = """    # Ensure default user and admin credentials are removed
    c.execute("DELETE FROM users WHERE username IN ('student', 'admin');")"""

for sfile in server_files:
    with open(sfile, 'r', encoding='utf-8') as f:
        content = f.read()
    if seed_code_block in content:
        content = content.replace(seed_code_block, clean_seed_block)
        print(f"  ✓ Replaced seed block in {sfile}")
    else:
        print(f"  ⚠️ seed_code_block not found in {sfile}")
    with open(sfile, 'w', encoding='utf-8') as f:
        f.write(content)

print("\n=== 3. UPDATING online_exam_mysql.sql & web-portal/online_exam_mysql.sql ===")
mysql_files = ['online_exam_mysql.sql', 'web-portal/online_exam_mysql.sql']
for mfile in mysql_files:
    with open(mfile, 'r', encoding='utf-8') as f:
        content = f.read()
    # Remove INSERT INTO `users` block
    content = re.sub(r'INSERT INTO `users`[^\;]+\;\n*', '', content)
    with open(mfile, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"  ✓ Cleaned user inserts from {mfile}")

print("\n=== 4. UPDATING index.html & web-portal/index.html ===")
html_files = ['index.html', 'web-portal/index.html']

old_defaults_block = """    // =========================================================================
    // STUDENT CANDIDATE REPOSITORY & USER STORE
    // =========================================================================
    const defaultStudent = {
      id: 'STU1001',
      username: 'student',
      name: 'Student Candidate',
      email: 'student@examportal.edu',
      password: 'password',
      role: 'student',
      roleLabel: 'Student Candidate',
      program: 'Computer Science & Engineering',
      avatar: '🎓'
    };

    const defaultAdmin = {
      id: 'ADM101',
      username: 'admin',
      name: 'System Administrator',
      email: 'admin@examportal.edu',
      password: 'admin',
      role: 'admin',
      roleLabel: 'System Administrator',
      department: 'Academic Affairs & Examination Board',
      avatar: '🛡️'
    };

    const registeredCandidates = {
      'student': defaultStudent,
      'stu1001': defaultStudent,
      'student1': defaultStudent,
      'alex': defaultStudent,
      'student@examportal.edu': defaultStudent
    };

    const registeredAdmins = {
      'admin': defaultAdmin,
      'adm101': defaultAdmin,
      'admin@examportal.edu': defaultAdmin
    };"""

new_defaults_block = """    // =========================================================================
    // STUDENT CANDIDATE REPOSITORY & USER STORE (Clean State - No Default Accounts)
    // =========================================================================
    const registeredCandidates = {};
    const registeredAdmins = {};"""

old_restore_block = """      // Ensure defaults are always active
      registeredCandidates['student'] = defaultStudent;
      registeredCandidates['stu1001'] = defaultStudent;
      registeredCandidates['student1'] = defaultStudent;
      registeredAdmins['admin'] = defaultAdmin;
      registeredAdmins['adm101'] = defaultAdmin;"""

new_restore_block = """      // Purge default and demo accounts from memory and storage
      ['student', 'stu1001', 'student1', 'alex', 'student@examportal.edu'].forEach(k => {
        delete registeredCandidates[k];
      });
      ['admin', 'adm101', 'admin@examportal.edu'].forEach(k => {
        delete registeredAdmins[k];
      });
      try {
        localStorage.setItem('online_exam_registered_students', JSON.stringify(registeredCandidates));
        localStorage.setItem('online_exam_registered_admins', JSON.stringify(registeredAdmins));
      } catch (e) { }"""

for hfile in html_files:
    with open(hfile, 'r', encoding='utf-8') as f:
        content = f.read()

    # A. Replace defaults block
    if old_defaults_block in content:
        content = content.replace(old_defaults_block, new_defaults_block)
        print(f"  ✓ Removed defaultStudent & defaultAdmin definitions from {hfile}")
    else:
        print(f"  ⚠️ old_defaults_block not found in {hfile}")

    # B. Replace restore block
    if old_restore_block in content:
        content = content.replace(old_restore_block, new_restore_block)
        print(f"  ✓ Removed defaults re-injection from localStorage loader in {hfile}")
    else:
        print(f"  ⚠️ old_restore_block not found in {hfile}")

    # C. Remove pre-filled values in Student form
    content = content.replace(
        'id="authUsername" class="auth-input" placeholder="Enter student ID, username or email" required value="student"',
        'id="authUsername" class="auth-input" placeholder="Enter student ID, username or email" required value=""'
    )
    content = content.replace(
        'id="authPassword" class="auth-input" placeholder="Enter password" required value="password"',
        'id="authPassword" class="auth-input" placeholder="Enter password" required value=""'
    )

    # D. Remove pre-filled values in Admin form
    content = content.replace(
        'id="authAdminUsername" class="auth-input" placeholder="Enter administrator ID or username" required value="admin"',
        'id="authAdminUsername" class="auth-input" placeholder="Enter administrator ID or username" required value=""'
    )
    content = content.replace(
        'id="authAdminPassword" class="auth-input" placeholder="Enter administrator password" required value="admin"',
        'id="authAdminPassword" class="auth-input" placeholder="Enter administrator password" required value=""'
    )

    # E. Empty demo filler functions
    old_fillers = """    function fillStudentCredentials() {
      const u = document.getElementById('authUsername');
      const p = document.getElementById('authPassword');
      if (u) u.value = 'student';
      if (p) p.value = 'password';
      showToastNotification("✓ Demo credentials filled: student / password");
    }

    function fillAdminCredentials() {
      const u = document.getElementById('authAdminUsername');
      const p = document.getElementById('authAdminPassword');
      if (u) u.value = 'admin';
      if (p) p.value = 'admin';
      showToastNotification("✓ Demo credentials filled: admin / admin");
    }"""

    new_fillers = """    function fillStudentCredentials() {
      const u = document.getElementById('authUsername');
      const p = document.getElementById('authPassword');
      if (u) u.value = '';
      if (p) p.value = '';
    }

    function fillAdminCredentials() {
      const u = document.getElementById('authAdminUsername');
      const p = document.getElementById('authAdminPassword');
      if (u) u.value = '';
      if (p) p.value = '';
    }"""

    if old_fillers in content:
        content = content.replace(old_fillers, new_fillers)
        print(f"  ✓ Cleaned fillStudentCredentials and fillAdminCredentials in {hfile}")

    with open(hfile, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {hfile} ({len(content)} bytes)")

print("\nDone removing default user and admin!")

