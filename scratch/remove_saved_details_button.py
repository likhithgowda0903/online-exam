import re

print("=== REMOVING SAVED DETAILS BUTTON ===")

student_btn_old = """                  <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin: 0;">USERNAME, ROLL NO. OR EMAIL</label>
                    <button type="button" id="btnStudentSavedDetails" onclick="toggleSavedDetailsDropdown('student', event)" style="background: rgba(56, 189, 248, 0.1); border: 1px solid rgba(56, 189, 248, 0.3); color: #38bdf8; font-size: 11px; font-weight: 700; padding: 2px 9px; border-radius: 6px; cursor: pointer; display: flex; align-items: center; gap: 4px; transition: background 0.2s;" onmouseover="this.style.background='rgba(56, 189, 248, 0.2)'" onmouseout="this.style.background='rgba(56, 189, 248, 0.1)'">
                      <span>📋</span> Saved Details
                    </button>
                  </div>"""

student_btn_new = """                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">USERNAME, ROLL NO. OR EMAIL</label>"""

admin_btn_old = """                  <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                    <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin: 0;">ADMINISTRATOR STAFF ID / USERNAME</label>
                    <button type="button" id="btnAdminSavedDetails" onclick="toggleSavedDetailsDropdown('admin', event)" style="background: rgba(56, 189, 248, 0.1); border: 1px solid rgba(56, 189, 248, 0.3); color: #38bdf8; font-size: 11px; font-weight: 700; padding: 2px 9px; border-radius: 6px; cursor: pointer; display: flex; align-items: center; gap: 4px; transition: background 0.2s;" onmouseover="this.style.background='rgba(56, 189, 248, 0.2)'" onmouseout="this.style.background='rgba(56, 189, 248, 0.1)'">
                      <span>📋</span> Saved Details
                    </button>
                  </div>"""

admin_btn_new = """                  <label style="display: block; font-size: 11px; font-weight: 700; color: #64748b; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 8px;">ADMINISTRATOR STAFF ID / USERNAME</label>"""

for fpath in ['index.html', 'web-portal/index.html']:
    print(f"Updating {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    if student_btn_old in content:
        content = content.replace(student_btn_old, student_btn_new)
        print("  ✓ Removed student 'Saved Details' button")
    else:
        print("  ⚠️ Could not find student_btn_old in", fpath)

    if admin_btn_old in content:
        content = content.replace(admin_btn_old, admin_btn_new)
        print("  ✓ Removed administrator 'Saved Details' button")
    else:
        print("  ⚠️ Could not find admin_btn_old in", fpath)

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)\n")

print("All files updated successfully!")

