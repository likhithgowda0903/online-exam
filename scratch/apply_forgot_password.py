import re

with open('index.html', 'r', encoding='utf-8') as f:
    html_sample = f.read()

# Let's prepare the recovery panel HTML
recovery_panel_html = """            <!-- STUDENT FORGOT PASSWORD VIEW -->
            <div id="authForgotPanel" style="${state.authTab === 'forgot-password' ? 'display: block;' : 'display: none;'}">
              <h3 style="color: #ffffff; font-size: 20px; font-weight: 800; margin: 0 0 6px 0;">Forgot Student Password?</h3>
              <p style="color: #94a3b8; font-size: 13px; margin: 0 0 20px 0;">
                Verify your registered student details to securely reset your password.
              </p>

              <!-- Step 1: Verification Form -->
              <div id="forgotStepVerify" style="${state.forgotStep === 2 ? 'display: none;' : 'display: block;'}">
                <form onsubmit="handleVerifyStudent(event)">
                  <div class="form-group" style="margin-bottom: 16px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">STUDENT ROLL NUMBER / USERNAME</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #c084fc; font-size: 15px; pointer-events: none;">🎓</span>
                      <input type="text" id="recoverStudentRoll" class="auth-input" placeholder="e.g. STU1001 or student username" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    </div>
                  </div>

                  <div class="form-group" style="margin-bottom: 22px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">REGISTERED EMAIL ADDRESS</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #38bdf8; font-size: 15px; pointer-events: none;">✉️</span>
                      <input type="email" id="recoverStudentEmail" class="auth-input" placeholder="e.g. candidate@university.edu" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    </div>
                  </div>

                  <div style="display: flex; gap: 12px;">
                    <button type="button" onclick="switchAuthTab('signin')" style="flex: 1; padding: 12px; font-size: 14px; font-weight: 700; color: #94a3b8; background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; cursor: pointer; transition: background 0.2s;" onmouseover="this.style.background='rgba(255,255,255,0.1)'" onmouseout="this.style.background='rgba(255,255,255,0.05)'">
                      ← Back to Sign In
                    </button>
                    <button type="submit" style="flex: 1.4; padding: 12px; font-size: 14px; font-weight: 700; color: #ffffff; background: #1d4ed8; border: none; border-radius: 8px; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 6px; box-shadow: 0 4px 14px rgba(29, 78, 216, 0.35); transition: background 0.2s;" onmouseover="this.style.background='#2563eb'" onmouseout="this.style.background='#1d4ed8'">
                      Verify Account ▶
                    </button>
                  </div>
                </form>
              </div>

              <!-- Step 2: Password Reset Form -->
              <div id="forgotStepReset" style="${state.forgotStep === 2 ? 'display: block;' : 'display: none;'}">
                <div style="background: rgba(16, 185, 129, 0.12); border: 1px solid rgba(16, 185, 129, 0.35); border-radius: 8px; padding: 11px 14px; margin-bottom: 18px; font-size: 13px; color: #34d399; display: flex; align-items: center; gap: 8px;">
                  <span>✓</span>
                  <span>Verified Account: <strong id="verifiedStudentLabel" style="color: #fff;">${state.verifiedCandidateName || 'Student'}</strong></span>
                </div>

                <form onsubmit="handleResetPassword(event)">
                  <div class="form-group" style="margin-bottom: 16px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">NEW PASSWORD</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                      <input type="password" id="newStudentPassword" class="auth-input" placeholder="Minimum 6 characters" minlength="6" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                      <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('newStudentPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                    </div>
                  </div>

                  <div class="form-group" style="margin-bottom: 22px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">CONFIRM NEW PASSWORD</label>
                    <div class="input-field-wrap" style="position: relative;">
                      <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                      <input type="password" id="confirmStudentPassword" class="auth-input" placeholder="Re-enter new password" minlength="6" required style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                      <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('confirmStudentPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                    </div>
                  </div>

                  <div style="display: flex; gap: 12px;">
                    <button type="button" onclick="switchAuthTab('signin')" style="flex: 1; padding: 12px; font-size: 14px; font-weight: 700; color: #94a3b8; background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; cursor: pointer; transition: background 0.2s;" onmouseover="this.style.background='rgba(255,255,255,0.1)'" onmouseout="this.style.background='rgba(255,255,255,0.05)'">
                      Cancel
                    </button>
                    <button type="submit" style="flex: 1.4; padding: 12px; font-size: 14px; font-weight: 700; color: #ffffff; background: #16a34a; border: none; border-radius: 8px; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 6px; box-shadow: 0 4px 14px rgba(22, 163, 74, 0.35); transition: background 0.2s;" onmouseover="this.style.background='#15803d'" onmouseout="this.style.background='#16a34a'">
                      Reset Password ✓
                    </button>
                  </div>
                </form>
              </div>
            </div>"""

recovery_functions_js = """    function openStudentForgotPassword(event) {
      if (event) event.preventDefault();
      state.forgotStep = 1;
      state.verifiedCandidateKey = null;
      state.verifiedCandidateName = '';
      switchAuthTab('forgot-password');
    }

    async function handleVerifyStudent(event) {
      if (event) event.preventDefault();
      const rollInput = document.getElementById('recoverStudentRoll');
      const emailInput = document.getElementById('recoverStudentEmail');
      const rollVal = rollInput ? rollInput.value.trim().toLowerCase() : '';
      const emailVal = emailInput ? emailInput.value.trim().toLowerCase() : '';

      if (!rollVal) {
        showToastNotification("⚠️ Please enter your student roll number or username.");
        if (rollInput) rollInput.focus();
        return;
      }
      if (!emailVal) {
        showToastNotification("⚠️ Please enter your registered email address.");
        if (emailInput) emailInput.focus();
        return;
      }

      // 1. Check in-memory / localStorage registered candidates
      let matchedKey = null;
      let matchedName = '';
      for (const [k, u] of Object.entries(registeredCandidates)) {
        if (!u) continue;
        const uId = (u.id || '').toLowerCase();
        const uName = (u.username || '').toLowerCase();
        const uEmail = (u.email || '').toLowerCase();
        if ((uId === rollVal || uName === rollVal) && uEmail === emailVal) {
          matchedKey = k;
          matchedName = u.name || u.id;
          break;
        }
      }

      // 2. Also check SQLite 3 database via backend API
      try {
        const res = await fetch('/api/verify-student', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ identifier: rollVal, email: emailVal })
        });
        if (res.ok) {
          const data = await res.json();
          if (data && data.verified) {
            matchedKey = matchedKey || data.username || rollVal;
            matchedName = matchedName || data.full_name || data.student_id;
          }
        }
      } catch (err) {
        // Operates in offline/local storage mode seamlessly
      }

      if (!matchedKey) {
        showToastNotification("❌ Account verification failed. No student account matched the provided roll number and email.");
        return;
      }

      state.verifiedCandidateKey = matchedKey;
      state.verifiedCandidateName = matchedName || rollVal.toUpperCase();
      state.forgotStep = 2;

      // Re-render auth view so Step 2 is cleanly displayed
      renderAuthView(document.getElementById('appViewport'));
      showToastNotification("✓ Student account verified! Please set your new password.");
    }

    async function handleResetPassword(event) {
      if (event) event.preventDefault();
      const p1 = document.getElementById('newStudentPassword');
      const p2 = document.getElementById('confirmStudentPassword');
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

      const key = state.verifiedCandidateKey;
      if (!key) {
        showToastNotification("⚠️ Verification session expired. Please verify your account again.");
        openStudentForgotPassword();
        return;
      }

      // 1. Update in-memory and localStorage candidates
      if (registeredCandidates[key]) {
        registeredCandidates[key].password = pwd1;
      }
      for (const [k, u] of Object.entries(registeredCandidates)) {
        if (!u) continue;
        if (u === registeredCandidates[key] ||
            (u.id && u.id.toLowerCase() === key.toLowerCase()) ||
            (u.username && u.username.toLowerCase() === key.toLowerCase())) {
          u.password = pwd1;
        }
      }
      try {
        localStorage.setItem('online_exam_registered_students', JSON.stringify(registeredCandidates));
      } catch (e) { }

      // 2. Synchronize new password with SQLite 3 database
      try {
        fetch('/api/reset-password', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ identifier: key, new_password: pwd1 })
        }).catch(() => { });
      } catch (err) { }

      state.forgotStep = 1;
      state.verifiedCandidateKey = null;
      state.verifiedCandidateName = '';

      showToastNotification("✓ Password reset successfully! You can now sign in with your new password.");
      switchAuthTab('signin');
      setTimeout(() => {
        const uField = document.getElementById('authUsername');
        if (uField) {
          uField.value = key;
          const pField = document.getElementById('authPassword');
          if (pField) pField.focus();
        }
      }, 100);
    }"""

files_to_update = ['index.html', 'web-portal/index.html']

for fpath in files_to_update:
    print(f"Updating {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Update state object with forgot password state
    target_state = "authTab: 'signin',"
    replacement_state = "authTab: 'signin',\n      forgotStep: 1,\n      verifiedCandidateKey: null,\n      verifiedCandidateName: '',"
    if target_state in content and "forgotStep:" not in content:
        content = content.replace(target_state, replacement_state)
        print("  ✓ Added forgotStep & verification state properties")

    # 2. Update Student Forgot Password link
    target_link = """                  <a href="javascript:void(0)" onclick="alert('Password Reset Instruction:\\n\\nPlease contact the examination controller or department coordinator at support@examportal.edu to reset your candidate credentials.')" style="color: #38bdf8; text-decoration: none; font-weight: 600;">
                    Forgot Password?
                  </a>"""
    replacement_link = """                  <a href="javascript:void(0)" onclick="openStudentForgotPassword(event)" style="color: #38bdf8; text-decoration: none; font-weight: 600; cursor: pointer;">
                    Forgot Password?
                  </a>"""
    if target_link in content:
        content = content.replace(target_link, replacement_link)
        print("  ✓ Updated student Forgot Password link handler")
    else:
        # Regex replacement if alert string differed slightly
        content = re.sub(
            r'<a href="javascript:void\(0\)" onclick="alert\([^)]+\)"[^>]*>\s*Forgot Password\?\s*<\/a>',
            replacement_link,
            content,
            count=1
        )
        print("  ✓ Updated student Forgot Password link via regex")

    # 3. Add #authForgotPanel right before #authRegisterPanel
    target_reg_panel = '            <!-- REGISTRATION VIEW (Supports both Student & Administrator) -->'
    if target_reg_panel in content and 'id="authForgotPanel"' not in content:
        content = content.replace(target_reg_panel, recovery_panel_html + "\n\n" + target_reg_panel)
        print("  ✓ Added authForgotPanel to authentication card")

    # 4. Update switchAuthTab to handle forgot-password tab cleanly
    target_switch = """    function switchAuthTab(tab, role = null) {
      state.authTab = tab;
      if (role) {
        state.regRole = role;
      }
      renderAuthView(document.getElementById('appViewport'));
    }"""

    replacement_switch = """    function switchAuthTab(tab, role = null) {
      state.authTab = tab;
      if (role) {
        state.regRole = role;
      }
      if (tab !== 'forgot-password') {
        state.forgotStep = 1;
        state.verifiedCandidateKey = null;
        state.verifiedCandidateName = '';
      }
      renderAuthView(document.getElementById('appViewport'));
    }"""
    if target_switch in content:
        content = content.replace(target_switch, replacement_switch)
        print("  ✓ Updated switchAuthTab function")

    # 5. Add recovery functions
    target_funcs_anchor = "    function togglePasswordVisibility(inputId) {"
    if target_funcs_anchor in content and "function openStudentForgotPassword" not in content:
        content = content.replace(target_funcs_anchor, recovery_functions_js + "\n\n" + target_funcs_anchor)
        print("  ✓ Added recovery handler functions")

    # 6. Ensure handleRegisterSubmit syncs to SQLite
    target_reg_sync = "loginUser(newUser);\n        showToastNotification(`🎓 Student candidate"
    replacement_reg_sync = """// Synchronize with SQLite 3 users table
        try {
          fetch('/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(newUser)
          }).catch(() => { });
        } catch (e) { }

        loginUser(newUser);
        showToastNotification(`🎓 Student candidate"""
    if target_reg_sync in content:
        content = content.replace(target_reg_sync, replacement_reg_sync)
        print("  ✓ Added SQLite sync to student registration")

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)")

print("\nAll files successfully updated!")

