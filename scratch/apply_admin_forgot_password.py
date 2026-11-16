import re

# 1. Update start_server.py and web-portal/start_server.py
server_verify_admin_code = """        # API: Verify Administrator Account for Password Recovery
        if path == '/api/verify-admin':
            content_len = int(self.headers.get('Content-Length', 0))
            post_body = self.rfile.read(content_len).decode('utf-8')
            try:
                payload = json.loads(post_body)
                ident = str(payload.get('identifier', '')).strip().lower()
                email = str(payload.get('email', '')).strip().lower()
                conn = get_db()
                c = conn.cursor()
                c.execute(\"\"\"
                    SELECT id, username, specific_id, full_name, email 
                    FROM users 
                    WHERE role = 'admin' 
                      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
                      AND LOWER(email) = ?
                \"\"\", (ident, ident, email))
                row = c.fetchone()
                conn.close()
                if row:
                    return self.send_json({
                        'verified': True,
                        'username': row['username'],
                        'admin_id': row['specific_id'],
                        'full_name': row['full_name']
                    })
                else:
                    return self.send_json({'verified': False, 'error': 'No matching administrator account found'}, 404)
            except Exception as e:
                return self.send_json({'verified': False, 'error': str(e)}, 400)
"""

for sfile in ['start_server.py', 'web-portal/start_server.py']:
    with open(sfile, 'r', encoding='utf-8') as f:
        content = f.read()

    if '/api/verify-admin' not in content:
        pos = content.find("        # API: Reset Student Password in SQLite 3")
        if pos != -1:
            content = content[:pos] + server_verify_admin_code + "\n" + content[pos:]
            print(f"  ✓ Added /api/verify-admin to {sfile}")
        else:
            print(f"  ⚠️ Could not find insert pos in {sfile}")

    # Update /api/reset-password to update by role or both
    old_reset_sql = """                c.execute(\"\"\"
                    UPDATE users 
                    SET password = ? 
                    WHERE role = 'student' 
                      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
                \"\"\", (new_pwd, ident, ident))"""
    new_reset_sql = """                role = str(payload.get('role', 'student')).strip().lower()
                if role == 'admin':
                    c.execute(\"\"\"
                        UPDATE users 
                        SET password = ? 
                        WHERE role = 'admin' 
                          AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
                    \"\"\", (new_pwd, ident, ident))
                else:
                    c.execute(\"\"\"
                        UPDATE users 
                        SET password = ? 
                        WHERE role = 'student' 
                          AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
                    \"\"\", (new_pwd, ident, ident))"""
    if old_reset_sql in content:
        content = content.replace(old_reset_sql, new_reset_sql)
        print(f"  ✓ Updated /api/reset-password to handle administrator role in {sfile}")

    with open(sfile, 'w', encoding='utf-8') as f:
        f.write(content)

# 2. Update index.html and web-portal/index.html
admin_recovery_panel_html = """            <!-- ADMINISTRATOR FORGOT PASSWORD VIEW -->
            <div id="authAdminForgotPanel" style="${state.authTab === 'forgot-password-admin' ? 'display: block;' : 'display: none;'}">
              <h3 style="color: #ffffff; font-size: 20px; font-weight: 800; margin: 0 0 6px 0;">Forgot Administrator Password?</h3>
              <p style="color: #94a3b8; font-size: 13px; margin: 0 0 20px 0;">
                Verify your official administrator credentials to securely reset your password.
              </p>

              <!-- Step 1: Verification Form -->
              <div id="adminForgotStepVerify" style="${state.adminForgotStep === 2 ? 'display: none;' : 'display: block;'}">
                <form onsubmit="handleVerifyAdmin(event)">
                  <div class="form-group" style="margin-bottom: 16px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">ADMINISTRATOR STAFF ID / USERNAME</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #38bdf8; font-size: 15px; pointer-events: none;">🛡️</span>
                      <input type="text" id="recoverAdminStaffId" class="auth-input" placeholder="e.g. ADM101 or administrator username" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    </div>
                  </div>

                  <div class="form-group" style="margin-bottom: 22px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">REGISTERED EMAIL ADDRESS</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #38bdf8; font-size: 15px; pointer-events: none;">✉️</span>
                      <input type="email" id="recoverAdminEmail" class="auth-input" placeholder="e.g. admin@examportal.edu" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    </div>
                  </div>

                  <div style="display: flex; gap: 12px;">
                    <button type="button" onclick="switchAuthTab('admin')" style="flex: 1; padding: 12px; font-size: 14px; font-weight: 700; color: #94a3b8; background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; cursor: pointer; transition: background 0.2s;" onmouseover="this.style.background='rgba(255,255,255,0.1)'" onmouseout="this.style.background='rgba(255,255,255,0.05)'">
                      ← Back to Sign In
                    </button>
                    <button type="submit" style="flex: 1.4; padding: 12px; font-size: 14px; font-weight: 700; color: #ffffff; background: #1d4ed8; border: none; border-radius: 8px; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 6px; box-shadow: 0 4px 14px rgba(29, 78, 216, 0.35); transition: background 0.2s;" onmouseover="this.style.background='#2563eb'" onmouseout="this.style.background='#1d4ed8'">
                      Verify Account ▶
                    </button>
                  </div>
                </form>
              </div>

              <!-- Step 2: Password Reset Form -->
              <div id="adminForgotStepReset" style="${state.adminForgotStep === 2 ? 'display: block;' : 'display: none;'}">
                <div style="background: rgba(16, 185, 129, 0.12); border: 1px solid rgba(16, 185, 129, 0.35); border-radius: 8px; padding: 11px 14px; margin-bottom: 18px; font-size: 13px; color: #34d399; display: flex; align-items: center; gap: 8px;">
                  <span>✓</span>
                  <span>Verified Administrator: <strong id="verifiedAdminLabel" style="color: #fff;">${state.verifiedAdminName || 'Administrator'}</strong></span>
                </div>

                <form onsubmit="handleResetAdminPassword(event)">
                  <div class="form-group" style="margin-bottom: 16px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">NEW PASSWORD</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                      <input type="password" id="newAdminPassword" class="auth-input" placeholder="Minimum 6 characters" minlength="6" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                      <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('newAdminPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                    </div>
                  </div>

                  <div class="form-group" style="margin-bottom: 22px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">CONFIRM NEW PASSWORD</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                      <input type="password" id="confirmAdminPassword" class="auth-input" placeholder="Re-enter new password" minlength="6" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                      <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('confirmAdminPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                    </div>
                  </div>

                  <div style="display: flex; gap: 12px;">
                    <button type="button" onclick="switchAuthTab('admin')" style="flex: 1; padding: 12px; font-size: 14px; font-weight: 700; color: #94a3b8; background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; cursor: pointer; transition: background 0.2s;" onmouseover="this.style.background='rgba(255,255,255,0.1)'" onmouseout="this.style.background='rgba(255,255,255,0.05)'">
                      Cancel
                    </button>
                    <button type="submit" style="flex: 1.4; padding: 12px; font-size: 14px; font-weight: 700; color: #ffffff; background: #16a34a; border: none; border-radius: 8px; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 6px; box-shadow: 0 4px 14px rgba(22, 163, 74, 0.35); transition: background 0.2s;" onmouseover="this.style.background='#15803d'" onmouseout="this.style.background='#16a34a'">
                      Reset Password ✓
                    </button>
                  </div>
                </form>
              </div>
            </div>"""

admin_recovery_functions_js = """    function openAdminForgotPassword(event) {
      if (event) event.preventDefault();
      state.adminForgotStep = 1;
      state.verifiedAdminKey = null;
      state.verifiedAdminName = '';
      switchAuthTab('forgot-password-admin');
    }

    async function handleVerifyAdmin(event) {
      if (event) event.preventDefault();
      const staffInput = document.getElementById('recoverAdminStaffId');
      const emailInput = document.getElementById('recoverAdminEmail');
      const staffVal = staffInput ? staffInput.value.trim().toLowerCase() : '';
      const emailVal = emailInput ? emailInput.value.trim().toLowerCase() : '';

      if (!staffVal) {
        showToastNotification("⚠️ Please enter your administrator Staff ID or username.");
        if (staffInput) staffInput.focus();
        return;
      }
      if (!emailVal) {
        showToastNotification("⚠️ Please enter your registered email address.");
        if (emailInput) emailInput.focus();
        return;
      }

      // 1. Check in-memory / localStorage registered administrators
      let matchedKey = null;
      let matchedName = '';
      for (const [k, a] of Object.entries(registeredAdmins)) {
        if (!a) continue;
        const aId = (a.id || '').toLowerCase();
        const aName = (a.username || '').toLowerCase();
        const aEmail = (a.email || '').toLowerCase();
        if ((aId === staffVal || aName === staffVal) && aEmail === emailVal) {
          matchedKey = k;
          matchedName = a.name || a.id;
          break;
        }
      }

      // 2. Also check SQLite 3 database via backend API
      try {
        const res = await fetch('/api/verify-admin', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ identifier: staffVal, email: emailVal })
        });
        if (res.ok) {
          const data = await res.json();
          if (data && data.verified) {
            matchedKey = matchedKey || data.username || staffVal;
            matchedName = matchedName || data.full_name || data.admin_id;
          }
        }
      } catch (err) {
        // Local/offline fallback
      }

      if (!matchedKey) {
        showToastNotification("❌ Account verification failed. No administrator account matched the provided details.");
        return;
      }

      state.verifiedAdminKey = matchedKey;
      state.verifiedAdminName = matchedName || staffVal.toUpperCase();
      state.adminForgotStep = 2;

      renderAuthView(document.getElementById('appViewport'));
      showToastNotification("✓ Administrator account verified! Please set your new password.");
    }

    async function handleResetAdminPassword(event) {
      if (event) event.preventDefault();
      const p1 = document.getElementById('newAdminPassword');
      const p2 = document.getElementById('confirmAdminPassword');
      const pwd1 = p1 ? p1.value : '';
      const pwd2 = p2 ? p2.value : '';

      if (!pwd1) {
        showToastNotification("⚠️ Please enter a new password.");
        if (p1) p1.focus();
        return;
      }
      if (pwd1.length < 6) {
        showToastNotification("⚠️ Password must be at least 6 characters long.");
        if (p1) p1.focus();
        return;
      }
      if (pwd1 !== pwd2) {
        showToastNotification("❌ Passwords do not match. Please re-enter.");
        if (p2) p2.focus();
        return;
      }

      const key = state.verifiedAdminKey;
      if (!key) {
        showToastNotification("⚠️ Verification session expired. Please verify your account again.");
        openAdminForgotPassword();
        return;
      }

      // 1. Update in registeredAdmins
      if (registeredAdmins[key]) {
        registeredAdmins[key].password = pwd1;
      }
      for (const [k, a] of Object.entries(registeredAdmins)) {
        if (!a) continue;
        if (a === registeredAdmins[key] ||
            (a.id && a.id.toLowerCase() === key.toLowerCase()) ||
            (a.username && a.username.toLowerCase() === key.toLowerCase())) {
          a.password = pwd1;
        }
      }
      try {
        localStorage.setItem('online_exam_registered_admins', JSON.stringify(registeredAdmins));
      } catch (e) { }

      // 2. Synchronize new password with SQLite 3 database
      try {
        fetch('/api/reset-password', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ identifier: key, new_password: pwd1, role: 'admin' })
        }).catch(() => { });
      } catch (err) { }

      state.adminForgotStep = 1;
      state.verifiedAdminKey = null;
      state.verifiedAdminName = '';

      showToastNotification("✓ Administrator password reset successfully! You can now sign in with your new password.");
      switchAuthTab('admin');
      setTimeout(() => {
        const uField = document.getElementById('authAdminUsername');
        if (uField) {
          uField.value = key;
          const pField = document.getElementById('authAdminPassword');
          if (pField) pField.focus();
        }
      }, 100);
    }"""

for fpath in ['index.html', 'web-portal/index.html']:
    print(f"Updating {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. State addition
    target_state = "verifiedCandidateName: '',"
    replacement_state = "verifiedCandidateName: '',\n      adminForgotStep: 1,\n      verifiedAdminKey: null,\n      verifiedAdminName: '',"
    if target_state in content and "adminForgotStep:" not in content:
        content = content.replace(target_state, replacement_state)
        print("  ✓ Added adminForgotStep & verification state properties")

    # 2. Update Admin Forgot Password link
    content = re.sub(
        r'<a href="javascript:void\(0\)" onclick="alert\(\'Administrator Access Recovery:[^)]+\)"[^>]*>\s*Forgot Password\?\s*<\/a>',
        '<a href="javascript:void(0)" onclick="openAdminForgotPassword(event)" style="color: #38bdf8; text-decoration: none; font-weight: 600; cursor: pointer;">Forgot Password?</a>',
        content
    )
    print("  ✓ Updated admin Forgot Password link handler")

    # 3. Add #authAdminForgotPanel right after #authForgotPanel
    pos_forgot_end = content.find("<!-- REGISTRATION VIEW (Supports both Student & Administrator) -->")
    if pos_forgot_end != -1 and 'id="authAdminForgotPanel"' not in content:
        content = content[:pos_forgot_end] + admin_recovery_panel_html + "\n\n            " + content[pos_forgot_end:]
        print("  ✓ Added authAdminForgotPanel to authentication card")

    # 4. Update switchAuthTab to handle forgot-password-admin tab
    target_switch = """      if (tab !== 'forgot-password') {
        state.forgotStep = 1;
        state.verifiedCandidateKey = null;
        state.verifiedCandidateName = '';
      }"""
    replacement_switch = """      if (tab !== 'forgot-password') {
        state.forgotStep = 1;
        state.verifiedCandidateKey = null;
        state.verifiedCandidateName = '';
      }
      if (tab !== 'forgot-password-admin') {
        state.adminForgotStep = 1;
        state.verifiedAdminKey = null;
        state.verifiedAdminName = '';
      }"""
    if target_switch in content:
        content = content.replace(target_switch, replacement_switch)
        print("  ✓ Updated switchAuthTab function")

    # 5. Add admin recovery functions
    target_funcs_anchor = "    function openStudentForgotPassword(event) {"
    if target_funcs_anchor in content and "function openAdminForgotPassword" not in content:
        content = content.replace(target_funcs_anchor, admin_recovery_functions_js + "\n\n" + target_funcs_anchor)
        print("  ✓ Added admin recovery handler functions")

    # 6. Synchronize admin registration to SQLite
    target_admin_reg = "loginUser(newAdmin);\n        showToastNotification(`🛡️ Administrator account"
    replacement_admin_reg = """// Synchronize with SQLite 3 users table
        try {
          fetch('/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(newAdmin)
          }).catch(() => { });
        } catch (e) { }

        loginUser(newAdmin);
        showToastNotification(`🛡️ Administrator account"""
    if target_admin_reg in content:
        content = content.replace(target_admin_reg, replacement_admin_reg)
        print("  ✓ Added SQLite sync to administrator registration")

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)")

print("\nAll files successfully updated with admin forgot password flow!")

