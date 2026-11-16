import re

print("=== PREPARING SAVED DETAILS ON CLICK FEATURE ===")

# Script to update index.html and web-portal/index.html
def update_file(filepath):
    print(f"Updating {filepath}...")
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Update Student Sign In username field in renderAuthView
    old_student_user_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">USERNAME, ROLL NO. OR EMAIL</label>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #c084fc; font-size: 15px; pointer-events: none;">👤</span>
                    <input type="text" id="authUsername" class="auth-input" placeholder="Enter student ID, username or email" required value="" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                  </div>
                </div>"""

    new_student_user_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin: 0;">USERNAME, ROLL NO. OR EMAIL</label>
                    <button type="button" id="btnStudentSavedDetails" onclick="toggleSavedDetailsDropdown('student', event)" style="background: rgba(56, 189, 248, 0.1); border: 1px solid rgba(56, 189, 248, 0.3); color: #38bdf8; font-size: 11px; font-weight: 700; padding: 2px 9px; border-radius: 6px; cursor: pointer; display: flex; align-items: center; gap: 4px; transition: background 0.2s;" onmouseover="this.style.background='rgba(56, 189, 248, 0.2)'" onmouseout="this.style.background='rgba(56, 189, 248, 0.1)'">
                      <span>📋</span> Saved Details
                    </button>
                  </div>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #c084fc; font-size: 15px; pointer-events: none;">👤</span>
                    <input type="text" id="authUsername" class="auth-input" placeholder="Enter student ID, username or email" required value="" autocomplete="off" readonly onfocus="unlockAuthInput(this)" onclick="handleAuthInputClick(this, 'student', event)" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    
                    <!-- Floating Saved Details Dropdown for Students -->
                    <div id="savedDetailsDropdown_student" class="saved-details-panel" style="display: none; position: absolute; left: 0; right: 0; top: calc(100% + 6px); background: #0b1329; border: 1px solid rgba(56, 189, 248, 0.4); border-radius: 8px; box-shadow: 0 12px 32px rgba(0, 0, 0, 0.7); z-index: 100; max-height: 250px; overflow-y: auto;">
                    </div>
                  </div>
                </div>"""

    if old_student_user_field in content:
        content = content.replace(old_student_user_field, new_student_user_field)
        print("  ✓ Updated student username field with on-click saved details")
    else:
        print("  ⚠️ Could not match old_student_user_field")

    # 2. Update Student Password field to be readonly on load until focused
    old_student_pwd_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">PASSWORD</label>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                    <input type="password" id="authPassword" class="auth-input" placeholder="Enter password" required value="" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('authPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                  </div>
                </div>"""

    new_student_pwd_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">PASSWORD</label>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                    <input type="password" id="authPassword" class="auth-input" placeholder="Enter password" required value="" autocomplete="new-password" readonly onfocus="unlockAuthInput(this)" onclick="unlockAuthInput(this)" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('authPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                  </div>
                </div>"""

    if old_student_pwd_field in content:
        content = content.replace(old_student_pwd_field, new_student_pwd_field)
        print("  ✓ Updated student password field with protected initial load")
    else:
        print("  ⚠️ Could not match old_student_pwd_field")

    # 3. Add ID to student Remember my session checkbox
    content = content.replace(
        '<input type="checkbox" checked style="accent-color: #1d4ed8; width: 15px; height: 15px;"> Remember my session',
        '<input type="checkbox" id="rememberStudentSession" checked style="accent-color: #1d4ed8; width: 15px; height: 15px;"> Remember my session',
        1
    )

    # 4. Update Administrator Sign In username field in renderAuthView
    old_admin_user_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">ADMINISTRATOR STAFF ID / USERNAME</label>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #38bdf8; font-size: 15px; pointer-events: none;">🛡️</span>
                    <input type="text" id="authAdminUsername" class="auth-input" placeholder="Enter administrator ID or username" required value="" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                  </div>
                </div>"""

    new_admin_user_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin: 0;">ADMINISTRATOR STAFF ID / USERNAME</label>
                    <button type="button" id="btnAdminSavedDetails" onclick="toggleSavedDetailsDropdown('admin', event)" style="background: rgba(56, 189, 248, 0.1); border: 1px solid rgba(56, 189, 248, 0.3); color: #38bdf8; font-size: 11px; font-weight: 700; padding: 2px 9px; border-radius: 6px; cursor: pointer; display: flex; align-items: center; gap: 4px; transition: background 0.2s;" onmouseover="this.style.background='rgba(56, 189, 248, 0.2)'" onmouseout="this.style.background='rgba(56, 189, 248, 0.1)'">
                      <span>📋</span> Saved Details
                    </button>
                  </div>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #38bdf8; font-size: 15px; pointer-events: none;">🛡️</span>
                    <input type="text" id="authAdminUsername" class="auth-input" placeholder="Enter administrator ID or username" required value="" autocomplete="off" readonly onfocus="unlockAuthInput(this)" onclick="handleAuthInputClick(this, 'admin', event)" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 14px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    
                    <!-- Floating Saved Details Dropdown for Administrators -->
                    <div id="savedDetailsDropdown_admin" class="saved-details-panel" style="display: none; position: absolute; left: 0; right: 0; top: calc(100% + 6px); background: #0b1329; border: 1px solid rgba(56, 189, 248, 0.4); border-radius: 8px; box-shadow: 0 12px 32px rgba(0, 0, 0, 0.7); z-index: 100; max-height: 250px; overflow-y: auto;">
                    </div>
                  </div>
                </div>"""

    if old_admin_user_field in content:
        content = content.replace(old_admin_user_field, new_admin_user_field)
        print("  ✓ Updated admin username field with on-click saved details")
    else:
        print("  ⚠️ Could not match old_admin_user_field")

    # 5. Update Administrator Password field to be readonly on load until focused
    old_admin_pwd_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">PASSWORD</label>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                    <input type="password" id="authAdminPassword" class="auth-input" placeholder="Enter administrator password" required value="" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('authAdminPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                  </div>
                </div>"""

    new_admin_pwd_field = """                <div class="form-group" style="margin-bottom: 16px;">
                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">PASSWORD</label>
                  <div class="input-field-wrap" style="position: relative;">
                    <span class="input-icon-lead" style="position: absolute; left: 14px; top: 50%; transform: translateY(-50%); color: #fbbf24; font-size: 15px; pointer-events: none;">🔒</span>
                    <input type="password" id="authAdminPassword" class="auth-input" placeholder="Enter administrator password" required value="" autocomplete="new-password" readonly onfocus="unlockAuthInput(this)" onclick="unlockAuthInput(this)" style="width: 100%; background: #070d1e; border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 8px; padding: 12px 40px 12px 42px; color: #ffffff; font-size: 14px; outline: none;">
                    <button type="button" class="input-toggle-pwd" onclick="togglePasswordVisibility('authAdminPassword')" style="position: absolute; right: 12px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #94a3b8; cursor: pointer; font-size: 16px;">👁️</button>
                  </div>
                </div>"""

    if old_admin_pwd_field in content:
        content = content.replace(old_admin_pwd_field, new_admin_pwd_field)
        print("  ✓ Updated admin password field with protected initial load")
    else:
        print("  ⚠️ Could not match old_admin_pwd_field")

    # 6. Add ID to admin Remember my session checkbox
    content = content.replace(
        '<input type="checkbox" checked style="accent-color: #1d4ed8; width: 15px; height: 15px;"> Remember my session',
        '<input type="checkbox" id="rememberAdminSession" checked style="accent-color: #1d4ed8; width: 15px; height: 15px;"> Remember my session',
        1
    )

    # 7. Add Saved Details JavaScript Logic functions
    saved_details_js = """    // =========================================================================
    // SAVED CREDENTIALS & ON-CLICK DETAILS CONTROLLER
    // Guarantees clean, blank inputs on website entry; displays saved details on click.
    // =========================================================================
    function unlockAuthInput(inputEl) {
      if (inputEl) {
        inputEl.removeAttribute('readonly');
      }
    }

    function handleAuthInputClick(inputEl, role, event) {
      unlockAuthInput(inputEl);
      if (event) event.stopPropagation();
      showSavedDetailsDropdown(role);
    }

    function toggleSavedDetailsDropdown(role, event) {
      if (event) event.stopPropagation();
      const dropId = role === 'admin' ? 'savedDetailsDropdown_admin' : 'savedDetailsDropdown_student';
      const dropdown = document.getElementById(dropId);
      if (dropdown && dropdown.style.display === 'block') {
        closeAllSavedDetailsDropdowns();
      } else {
        showSavedDetailsDropdown(role);
      }
    }

    function closeAllSavedDetailsDropdowns() {
      const d1 = document.getElementById('savedDetailsDropdown_student');
      const d2 = document.getElementById('savedDetailsDropdown_admin');
      if (d1) d1.style.display = 'none';
      if (d2) d2.style.display = 'none';
    }

    function getSavedAccountsForRole(role) {
      const accounts = [];
      const seen = new Set();

      if (role === 'admin') {
        // 1. Check remembered admin session login
        try {
          const rem = localStorage.getItem('online_exam_saved_admin_login');
          if (rem) {
            const p = JSON.parse(rem);
            const k = (p.identifier || p.username || p.id || '').toLowerCase();
            if (k && !seen.has(k)) {
              seen.add(k);
              accounts.push({
                identifier: p.identifier || p.username || p.id,
                password: p.password || '',
                name: p.name || p.identifier,
                id: p.id || p.identifier,
                email: p.email || '',
                roleLabel: 'Remembered Administrator',
                isRemembered: true
              });
            }
          }
        } catch (e) { }

        // 2. Check registeredAdmins
        for (const [k, a] of Object.entries(registeredAdmins)) {
          if (!a) continue;
          const ident = a.username || a.id || k;
          const keyLower = ident.toLowerCase();
          if (!seen.has(keyLower)) {
            seen.add(keyLower);
            accounts.push({
              identifier: ident,
              password: a.password || '',
              name: a.name || ident,
              id: a.id || ident,
              email: a.email || '',
              roleLabel: a.roleLabel || 'Registered Administrator',
              isRemembered: false
            });
          }
        }
      } else {
        // 1. Check remembered student session login
        try {
          const rem = localStorage.getItem('online_exam_saved_student_login');
          if (rem) {
            const p = JSON.parse(rem);
            const k = (p.identifier || p.username || p.id || '').toLowerCase();
            if (k && !seen.has(k)) {
              seen.add(k);
              accounts.push({
                identifier: p.identifier || p.username || p.id,
                password: p.password || '',
                name: p.name || p.identifier,
                id: p.id || p.identifier,
                email: p.email || '',
                roleLabel: 'Remembered Student',
                isRemembered: true
              });
            }
          }
        } catch (e) { }

        // 2. Check registeredCandidates
        for (const [k, c] of Object.entries(registeredCandidates)) {
          if (!c) continue;
          const ident = c.username || c.id || k;
          const keyLower = ident.toLowerCase();
          if (!seen.has(keyLower)) {
            seen.add(keyLower);
            accounts.push({
              identifier: ident,
              password: c.password || '',
              name: c.name || ident,
              id: c.id || ident,
              email: c.email || '',
              roleLabel: c.roleLabel || 'Registered Candidate',
              isRemembered: false
            });
          }
        }
      }
      return accounts;
    }

    function showSavedDetailsDropdown(role) {
      closeAllSavedDetailsDropdowns();
      const dropId = role === 'admin' ? 'savedDetailsDropdown_admin' : 'savedDetailsDropdown_student';
      const dropdown = document.getElementById(dropId);
      if (!dropdown) return;

      const accounts = getSavedAccountsForRole(role);

      let html = `
        <div style="padding: 10px 14px; border-bottom: 1px solid rgba(255, 255, 255, 0.08); display: flex; justify-content: space-between; align-items: center; background: rgba(15, 23, 42, 0.6);">
          <span style="font-size: 11px; font-weight: 700; color: #38bdf8; text-transform: uppercase; letter-spacing: 0.06em; display: flex; align-items: center; gap: 6px;">
            <span>📋</span> Saved Details (Click to Fill)
          </span>
          <button type="button" onclick="closeAllSavedDetailsDropdowns()" style="background: transparent; border: none; color: #94a3b8; font-size: 13px; cursor: pointer; padding: 2px 6px;">✕</button>
        </div>
      `;

      if (accounts.length === 0) {
        html += `
          <div style="padding: 18px 14px; text-align: center; color: #94a3b8; font-size: 13px;">
            <div style="font-size: 20px; margin-bottom: 6px;">ℹ️</div>
            <div>No saved account details found yet.</div>
            <div style="font-size: 11px; color: #64748b; margin-top: 4px;">Sign Up to create an account, or log in with 'Remember my session' enabled.</div>
          </div>
        `;
      } else {
        html += `<div style="padding: 6px;">`;
        accounts.forEach(acc => {
          const avatarIcon = role === 'admin' ? '🛡️' : '🎓';
          const badgeText = acc.isRemembered ? '● Saved Session' : '● Registered';
          const badgeColor = acc.isRemembered ? '#38bdf8' : '#34d399';
          html += `
            <div class="saved-account-row" onclick="selectSavedAccount('${acc.identifier.replace(/'/g, "\\'")}', '${(acc.password || '').replace(/'/g, "\\'")}', '${role}', '${(acc.name || acc.identifier).replace(/'/g, "\\'")}', event)" style="padding: 10px 12px; border-radius: 6px; cursor: pointer; display: flex; align-items: center; justify-content: space-between; gap: 10px; transition: background 0.15s; margin-bottom: 4px;" onmouseover="this.style.background='rgba(30, 58, 138, 0.35)'" onmouseout="this.style.background='transparent'">
              <div style="display: flex; align-items: center; gap: 10px; overflow: hidden;">
                <div style="width: 32px; height: 32px; border-radius: 8px; background: rgba(56, 189, 248, 0.15); display: flex; align-items: center; justify-content: center; font-size: 16px; flex-shrink: 0;">
                  ${avatarIcon}
                </div>
                <div style="overflow: hidden; text-align: left;">
                  <div style="color: #ffffff; font-size: 13px; font-weight: 700; white-space: nowrap; overflow: hidden; text-overflow: ellipsis;">
                    ${acc.name}
                  </div>
                  <div style="color: #94a3b8; font-size: 11px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis;">
                    ID: <strong style="color: #cbd5e1;">${acc.id || acc.identifier}</strong> ${acc.email ? '&bull; ' + acc.email : ''}
                  </div>
                </div>
              </div>
              <div style="flex-shrink: 0; text-align: right;">
                <span style="font-size: 10px; font-weight: 700; color: ${badgeColor}; display: block; margin-bottom: 2px;">${badgeText}</span>
                <span style="font-size: 11px; color: #38bdf8; font-weight: 700;">Fill ▶</span>
              </div>
            </div>
          `;
        });
        html += `</div>`;
      }

      dropdown.innerHTML = html;
      dropdown.style.display = 'block';
    }

    function selectSavedAccount(identifier, password, role, name, event) {
      if (event) event.stopPropagation();
      if (role === 'admin') {
        const u = document.getElementById('authAdminUsername');
        const p = document.getElementById('authAdminPassword');
        if (u) {
          unlockAuthInput(u);
          u.value = identifier;
        }
        if (p && password) {
          unlockAuthInput(p);
          p.value = password;
        }
      } else {
        const u = document.getElementById('authUsername');
        const p = document.getElementById('authPassword');
        if (u) {
          unlockAuthInput(u);
          u.value = identifier;
        }
        if (p && password) {
          unlockAuthInput(p);
          p.value = password;
        }
      }
      closeAllSavedDetailsDropdowns();
      showToastNotification(`✓ Filled saved details for ${name || identifier}`);
    }

    function enforceCleanAuthFieldsOnEntry() {
      ['authUsername', 'authPassword', 'authAdminUsername', 'authAdminPassword'].forEach(id => {
        const el = document.getElementById(id);
        if (el) {
          el.value = '';
          el.setAttribute('readonly', 'readonly');
        }
      });
      closeAllSavedDetailsDropdowns();
    }

    // Close saved details dropdown when clicking outside
    document.addEventListener('click', function(e) {
      const p1 = document.getElementById('savedDetailsDropdown_student');
      const p2 = document.getElementById('savedDetailsDropdown_admin');
      const b1 = document.getElementById('btnStudentSavedDetails');
      const b2 = document.getElementById('btnAdminSavedDetails');
      const u1 = document.getElementById('authUsername');
      const u2 = document.getElementById('authAdminUsername');

      const isInside = (p1 && p1.contains(e.target)) ||
                       (p2 && p2.contains(e.target)) ||
                       (b1 && b1.contains(e.target)) ||
                       (b2 && b2.contains(e.target)) ||
                       (u1 && u1 === e.target) ||
                       (u2 && u2 === e.target);
      if (!isInside) {
        closeAllSavedDetailsDropdowns();
      }
    });
"""

    if "function unlockAuthInput" not in content:
        # Place before fillStudentCredentials
        pos_insert = content.find("    function fillStudentCredentials()")
        if pos_insert != -1:
            content = content[:pos_insert] + saved_details_js + "\n\n" + content[pos_insert:]
            print("  ✓ Added saved details controller functions and click-outside handler")
        else:
            print("  ⚠️ Could not find insert pos for saved details functions")

    # 8. Update renderAuthView to invoke enforceCleanAuthFieldsOnEntry()
    old_render_end = "renderAuthView(container) {\n      container.innerHTML = `"
    if old_render_end in content:
        content = content.replace(
            "renderAuthView(container) {\n      container.innerHTML = `",
            "renderAuthView(container) {\n      container.innerHTML = `"
        )
    # At the end of renderAuthView:
    # Look for where renderAuthView finishes or call enforceCleanAuthFieldsOnEntry in showView('auth')
    pos_show_auth = content.find("case 'auth':")
    if pos_show_auth != -1 and "enforceCleanAuthFieldsOnEntry();" not in content:
        content = content.replace(
            "renderAuthView(container);\n          break;",
            "renderAuthView(container);\n          enforceCleanAuthFieldsOnEntry();\n          break;"
        )
        print("  ✓ Added enforceCleanAuthFieldsOnEntry() to showView('auth')")

    # 9. Update handleLoginSubmit to remember session if checked
    student_remember_logic = """      // Store or clear remembered student session
      try {
        const remCheck = document.getElementById('rememberStudentSession');
        if (remCheck && remCheck.checked) {
          localStorage.setItem('online_exam_saved_student_login', JSON.stringify({
            identifier: authenticatedUser.username || authenticatedUser.id,
            password: cleanPwd,
            name: authenticatedUser.name,
            id: authenticatedUser.id,
            email: authenticatedUser.email
          }));
        } else {
          localStorage.removeItem('online_exam_saved_student_login');
        }
      } catch (e) { }

      loginUser(authenticatedUser);"""

    if "loginUser(authenticatedUser);" in content and "online_exam_saved_student_login" not in content:
        content = content.replace("loginUser(authenticatedUser);", student_remember_logic)
        print("  ✓ Added student session remember persistence on login")

    # 10. Update handleAdminLoginSubmit to remember session if checked
    admin_remember_logic = """      // Store or clear remembered administrator session
      try {
        const remAdminCheck = document.getElementById('rememberAdminSession');
        if (remAdminCheck && remAdminCheck.checked) {
          localStorage.setItem('online_exam_saved_admin_login', JSON.stringify({
            identifier: matchedAdmin.username || matchedAdmin.id,
            password: cleanAdminPwd,
            name: matchedAdmin.name,
            id: matchedAdmin.id,
            email: matchedAdmin.email,
            department: matchedAdmin.department || matchedAdmin.dept || 'Administration'
          }));
        } else {
          localStorage.removeItem('online_exam_saved_admin_login');
        }
      } catch (e) { }

      loginUser(matchedAdmin);"""

    if "loginUser(matchedAdmin);" in content and "online_exam_saved_admin_login" not in content:
        content = content.replace("loginUser(matchedAdmin);", admin_remember_logic)
        print("  ✓ Added administrator session remember persistence on login")

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {filepath} ({len(content)} bytes)\n")

for f in ['index.html', 'web-portal/index.html']:
    update_file(f)

print("All files updated successfully!")

