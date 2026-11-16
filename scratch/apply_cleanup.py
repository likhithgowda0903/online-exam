import re

target_files = ['index.html', 'web-portal/index.html']

for fpath in target_files:
    print(f"Cleaning {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Remove #navDatabaseBtn from top navbar
    target_nav_btn = """        <button class="btn-action-outline" id="navDatabaseBtn"
          title="Open Actual Project Database (SQLite 3 - online_exam.db) in DB Browser for SQLite"
          onclick="handleOpenDatabaseClick()"
          style="display: inline-flex; align-items: center; gap: 6px; padding: 7px 12px; font-size: var(--fs-xs); font-weight: 700; border: 1px solid rgba(56, 189, 248, 0.35); background: rgba(15, 23, 42, 0.6); color: #38bdf8; border-radius: var(--radius-sm); cursor: pointer;">
          <span>🗄️</span>
          <span>Database</span>
        </button>"""
    if target_nav_btn in content:
        content = content.replace(target_nav_btn, "")
        print("  ✓ Removed #navDatabaseBtn from top navbar")
    else:
        # Regex fallback in case of whitespace differences
        content = re.sub(r'\s*<button class="btn-action-outline" id="navDatabaseBtn"[^>]*>[\s\S]*?<\/button>', '', content)
        print("  ✓ Removed #navDatabaseBtn via regex")

    # 2. Remove Database button from Admin Dashboard Header
    target_admin_db_btn = """                <button class="btn-action-outline" style="border-color: #3b82f6; color: #93c5fd; display: inline-flex; align-items: center; gap: 6px;" onclick="handleOpenDatabaseClick()" title="Open actual project database in DB Browser for SQLite">
                  🗄️ Database
                </button>"""
    if target_admin_db_btn in content:
        content = content.replace(target_admin_db_btn, "")
        print("  ✓ Removed Database button from Admin Dashboard header")

    # 3. Remove Database item from Admin Sidebar
    target_sidebar_db = """          { id: 'settings', label: 'System Settings', icon: '⚙️' },
          { id: 'database', label: 'Open Database', icon: '🗄️', action: 'handleOpenDatabaseClick()' }"""
    replacement_sidebar_db = """          { id: 'settings', label: 'System Settings', icon: '⚙️' }"""
    if target_sidebar_db in content:
        content = content.replace(target_sidebar_db, replacement_sidebar_db)
        print("  ✓ Removed Database nav item from Admin Sidebar")

    # 4. Strict login validation without phantom auto-created default users
    old_login_block = """      if (!authenticatedUser) {
        // Auto-create and authenticate student seamlessly
        const autoUser = {
          id: usernameInput.startsWith('stu') ? usernameInput.toUpperCase() : 'STU' + Math.floor(1000 + Math.random() * 9000),
          username: usernameInput,
          name: usernameInput.charAt(0).toUpperCase() + usernameInput.slice(1),
          email: usernameInput + '@student.edu',
          password: passwordInput || 'password',
          role: 'student',
          roleLabel: 'Student Candidate',
          program: 'Computer Science',
          avatar: '🎓'
        };
        registeredCandidates[usernameInput.toLowerCase()] = autoUser;
        registeredCandidates[autoUser.id.toLowerCase()] = autoUser;
        try {
          localStorage.setItem('online_exam_registered_students', JSON.stringify(registeredCandidates));
        } catch (e) { }
        loginUser(autoUser);
        return;
      }

      // Flexible password verification (accepts Student@123, password, student, etc.)
      const cleanPwd = passwordInput ? passwordInput.trim() : '';
      const allowedStudentPasswords = [
        authenticatedUser.password,
        'Student@123',
        'student@123',
        'password',
        'student',
        'student123',
        '123456',
        'admin',
        'Admin@123',
        'admin123'
      ].filter(Boolean);

      const isValidStudentPwd = allowedStudentPasswords.some(
        p => p === cleanPwd || p.toLowerCase() === cleanPwd.toLowerCase()
      );

      if (!isValidStudentPwd) {
        // Automatically accept and sync user password so they are never blocked
        authenticatedUser.password = cleanPwd;
        try {
          localStorage.setItem('online_exam_registered_students', JSON.stringify(registeredCandidates));
        } catch (e) { }
      }

      loginUser(authenticatedUser);"""

    new_login_block = """      if (!authenticatedUser) {
        showToastNotification("⚠️ Candidate account not found. Please click 'Sign Up' below to create your account.");
        if (userField) userField.focus();
        return;
      }

      const cleanPwd = passwordInput ? passwordInput.trim() : '';
      if (authenticatedUser.password !== cleanPwd) {
        showToastNotification("❌ Incorrect password. Please try again.");
        if (pwdField) pwdField.focus();
        return;
      }

      loginUser(authenticatedUser);"""

    if old_login_block in content:
        content = content.replace(old_login_block, new_login_block)
        print("  ✓ Replaced handleLoginSubmit with strict authentication")
    else:
        print("  ⚠️ old_login_block not matched!")

    # 5. Strict admin login validation without phantom auto-created admin users
    old_admin_login_block = """      if (!matchedAdmin) {
        const autoAdmin = {
          id: usernameInput.startsWith('adm') ? usernameInput.toUpperCase() : 'ADM' + Math.floor(100 + Math.random() * 900),
          username: usernameInput,
          name: 'Administrator (' + usernameInput + ')',
          email: usernameInput + '@examportal.edu',
          password: passwordInput || 'admin',
          role: 'admin',
          roleLabel: 'System Administrator',
          department: 'Academic Affairs',
          avatar: '🛡️'
        };
        registeredAdmins[usernameInput.toLowerCase()] = autoAdmin;
        registeredAdmins[autoAdmin.id.toLowerCase()] = autoAdmin;
        try {
          localStorage.setItem('online_exam_registered_admins', JSON.stringify(registeredAdmins));
        } catch (e) { }
        loginUser(autoAdmin);
        return;
      }

      // Flexible password verification (accepts Admin@123, admin, admin123, etc.)
      const cleanAdminPwd = passwordInput ? passwordInput.trim() : '';
      const allowedAdminPasswords = [
        matchedAdmin.password,
        'Admin@123',
        'admin@123',
        'admin',
        'admin123',
        'password',
        '123456',
        'Student@123'
      ].filter(Boolean);

      const isValidAdminPwd = allowedAdminPasswords.some(
        p => p === cleanAdminPwd || p.toLowerCase() === cleanAdminPwd.toLowerCase()
      );

      if (!isValidAdminPwd) {
        // Automatically accept and sync admin password so they are never blocked
        matchedAdmin.password = cleanAdminPwd;
        try {
          localStorage.setItem('online_exam_registered_admins', JSON.stringify(registeredAdmins));
        } catch (e) { }
      }

      loginUser(matchedAdmin);"""

    new_admin_login_block = """      if (!matchedAdmin) {
        showToastNotification("⚠️ Administrator account not found. Please click 'Sign Up' below to create an administrator account.");
        if (userField) userField.focus();
        return;
      }

      const cleanAdminPwd = passwordInput ? passwordInput.trim() : '';
      if (matchedAdmin.password !== cleanAdminPwd) {
        showToastNotification("❌ Incorrect administrator password. Please try again.");
        if (pwdField) pwdField.focus();
        return;
      }

      loginUser(matchedAdmin);"""

    if old_admin_login_block in content:
        content = content.replace(old_admin_login_block, new_admin_login_block)
        print("  ✓ Replaced handleAdminLoginSubmit with strict authentication")
    else:
        print("  ⚠️ old_admin_login_block not matched!")

    # 6. Replace fallback email in admin profile
    content = content.replace(
        "${state.user ? (state.user.email || 'admin@examportal.edu') : 'admin@examportal.edu'}",
        "${state.user ? (state.user.email || '') : ''}"
    )
    content = content.replace(
        "${state.user ? state.user.email : 'admin@examportal.edu'}",
        "${state.user ? state.user.email : ''}"
    )

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)")

print("\nCleanup completed successfully!")

