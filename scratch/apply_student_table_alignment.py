import re

print("=== APPLYING STUDENT CANDIDATE REGISTRY TABLE ALIGNMENT FIX ===")

# 1. Update start_server.py and web-portal/start_server.py
server_students_endpoint = """        # API: Get Registered Students from SQLite
        if path == '/api/students':
            conn = get_db()
            c = conn.cursor()
            c.execute("SELECT id, username, specific_id, full_name, email, role, created_at FROM users WHERE role = 'student'")
            rows = c.fetchall()
            students = []
            for r in rows:
                students.append({
                    'id': r['specific_id'] or r['username'],
                    'username': r['username'],
                    'name': r['full_name'] or r['username'],
                    'email': r['email'] or '',
                    'role': 'student',
                    'program': 'Computer Science',
                    'created_at': r['created_at']
                })
            conn.close()
            return self.send_json(students)
"""

for sfile in ['start_server.py', 'web-portal/start_server.py']:
    with open(sfile, 'r', encoding='utf-8') as f:
        scontent = f.read()

    if "if path == '/api/students':" not in scontent:
        pos = scontent.find("        # API: Get Question Pools from SQLite")
        if pos != -1:
            scontent = scontent[:pos] + server_students_endpoint + "\n" + scontent[pos:]
            print(f"  ✓ Added /api/students endpoint to {sfile}")
            with open(sfile, 'w', encoding='utf-8') as f:
                f.write(scontent)

# 2. CSS to add to index.html and web-portal/index.html
student_table_css = """    /* =========================================================================
       ADMINISTRATOR STUDENT CANDIDATE REGISTRY TABLE STYLES
       Guarantees recommended column proportions (14%, 20%, 20%, 13%, 18%, 15%),
       header-data alignment, vertical stacking of name/email & responsive scroll
       ========================================================================= */
    .admin-student-table {
      width: 100%;
      min-width: 900px;
      border-collapse: separate;
      border-spacing: 0;
      table-layout: fixed;
    }

    .admin-student-table th {
      background: rgba(15, 23, 42, 0.85);
      color: #94a3b8;
      font-size: var(--fs-xs);
      font-weight: 700;
      text-transform: uppercase;
      letter-spacing: 0.05em;
      padding: 14px 12px;
      border-bottom: 1px solid var(--border-color);
      vertical-align: middle;
      white-space: nowrap;
      box-sizing: border-box;
    }

    .admin-student-table td {
      padding: 16px 12px;
      border-bottom: 1px solid rgba(255, 255, 255, 0.06);
      font-size: var(--fs-sm);
      color: #e2e8f0;
      vertical-align: middle;
      box-sizing: border-box;
    }

    .admin-student-table tr:hover td {
      background: rgba(30, 58, 138, 0.14);
    }

    .admin-student-col-roll {
      width: 14%;
      text-align: center;
    }

    .admin-student-col-name {
      width: 20%;
      text-align: left;
    }

    .admin-student-col-program {
      width: 20%;
      text-align: left;
    }

    .admin-student-col-exams {
      width: 13%;
      text-align: center;
    }

    .admin-student-col-avg {
      width: 18%;
      text-align: center;
    }

    .admin-student-col-status {
      width: 15%;
      text-align: center;
    }
"""

new_student_tab_html = """          <!-- TAB CONTENT 3: STUDENTS -->
          ${activeTab === 'students' ? `
            <div class="card-frame" style="padding: 22px; background: var(--bg-card); border: 1px solid var(--border-color); border-radius: var(--radius-md);">
              <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 18px; flex-wrap: wrap; gap: 12px;">
                <div>
                  <h3 style="color: #fff; font-size: var(--fs-lg); font-weight: 700;">Student Candidate Registry</h3>
                  <p style="color: var(--text-dim); font-size: var(--fs-sm); margin-top: 2px;">Directory of enrolled candidate profiles, degree programs, and exam attempt records.</p>
                </div>
                <div style="display: flex; gap: 8px;">
                  <span class="badge-pill green" style="font-weight: 700; font-size: var(--fs-xs);">${getUniqueRegisteredStudents().length} Candidate${getUniqueRegisteredStudents().length === 1 ? '' : 's'} Enrolled</span>
                </div>
              </div>

              <div class="table-responsive-admin">
                <table class="admin-student-table">
                  <colgroup>
                    <col style="width: 14%;">
                    <col style="width: 20%;">
                    <col style="width: 20%;">
                    <col style="width: 13%;">
                    <col style="width: 18%;">
                    <col style="width: 15%;">
                  </colgroup>
                  <thead>
                    <tr>
                      <th class="admin-student-col-roll" style="text-align: center;">Student Roll ID</th>
                      <th class="admin-student-col-name" style="text-align: left;">Candidate Name</th>
                      <th class="admin-student-col-program" style="text-align: left;">Academic Program</th>
                      <th class="admin-student-col-exams" style="text-align: center;">Exams Taken</th>
                      <th class="admin-student-col-avg" style="text-align: center;">Institutional Avg</th>
                      <th class="admin-student-col-status" style="text-align: center;">Status</th>
                    </tr>
                  </thead>
                  <tbody>
                    ${getUniqueRegisteredStudents().length === 0 ? `
                      <tr>
                        <td colspan="6" style="text-align: center; padding: 40px 20px; color: var(--text-muted);">
                          <div style="font-size: 2.2rem; margin-bottom: 10px;">👥</div>
                          <div style="font-size: 1.1rem; font-weight: 800; color: #fff;">No Student Candidates Registered</div>
                          <div style="font-size: 0.85rem; margin-top: 6px; color: #94a3b8;">Enrolled candidates will automatically appear here once registered via the Student Portal.</div>
                        </td>
                      </tr>
                    ` : getUniqueRegisteredStudents().map(s => {
                      const studentKey = (s.id || s.username || '').toLowerCase();
                      const attempts = (state.history || []).filter(h => (h.studentId && h.studentId.toLowerCase() === studentKey) || (h.userId && h.userId.toLowerCase() === studentKey));
                      const avg = attempts.length > 0 ? `${Math.round(attempts.reduce((a, b) => a + (b.pct || 0), 0) / attempts.length)}%` : 'Pending Exam';
                      return `
                        <tr>
                          <td class="admin-student-col-roll" style="text-align: center;">
                            <span class="badge-pill blue" style="font-weight: 700; font-size: var(--fs-xs);">${s.id || s.username}</span>
                          </td>
                          <td class="admin-student-col-name" style="text-align: left;">
                            <div style="font-weight: 800; color: #fff; font-size: var(--fs-base); line-height: 1.3;">${s.name}</div>
                            <div style="font-size: var(--fs-xs); color: #94a3b8; margin-top: 3px;">${s.email || 'No email registered'}</div>
                          </td>
                          <td class="admin-student-col-program" style="text-align: left;">
                            <span style="color: #cbd5e1; font-size: var(--fs-sm); font-weight: 600;">${s.program || 'Computer Science'}</span>
                          </td>
                          <td class="admin-student-col-exams" style="text-align: center;">
                            <strong style="color: #fff; font-size: var(--fs-base);">${attempts.length}</strong> <span style="color: var(--text-muted); font-size: var(--fs-xs);">Exam${attempts.length === 1 ? '' : 's'}</span>
                          </td>
                          <td class="admin-student-col-avg" style="text-align: center;">
                            ${attempts.length > 0 
                              ? `<strong style="color: #34d399; font-size: var(--fs-base);">${avg}</strong>` 
                              : `<span class="badge-pill" style="background: rgba(148, 163, 184, 0.12); color: #94a3b8; font-size: var(--fs-xs); font-weight: 600;">Pending Exam</span>`}
                          </td>
                          <td class="admin-student-col-status" style="text-align: center;">
                            <span class="badge-pill green" style="font-weight: 700; font-size: var(--fs-xs);">Enrolled</span>
                          </td>
                        </tr>
                      `;
                    }).join('')}
                  </tbody>
                </table>
              </div>
            </div>
          ` : ''}"""

sqlite_student_sync_code = """        // Fetch Students from SQLite table 'users'
        try {
          const studentsRes = await fetch('/api/students');
          if (studentsRes.ok) {
            const dbStudents = await studentsRes.json();
            if (Array.isArray(dbStudents)) {
              dbStudents.forEach(s => {
                const sKey = (s.username || s.id).toLowerCase();
                if (!registeredCandidates[sKey]) {
                  registeredCandidates[sKey] = s;
                }
              });
            }
          }
        } catch (e) { }
"""

for fpath in ['index.html', 'web-portal/index.html']:
    print(f"Updating {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Add CSS
    if ".admin-student-table" not in content:
        pos_table_css = content.find("    /* TABLES & CONTROLS */")
        if pos_table_css != -1:
            content = content[:pos_table_css] + student_table_css + "\n" + content[pos_table_css:]
            print("  ✓ Added dedicated admin student table CSS")
        else:
            print("  ⚠️ Could not find insert pos for student table CSS")

    # 2. Replace the student tab table
    pattern = re.compile(
        r'<!-- TAB CONTENT 3: STUDENTS -->\s*\$\{activeTab === \'students\' \? `[\s\S]*?` : \'\'\}',
        re.MULTILINE
    )
    if pattern.search(content):
        content = pattern.sub(new_student_tab_html, content, count=1)
        print("  ✓ Replaced student candidate registry table with perfectly aligned structure")
    else:
        print("  ⚠️ Could not regex match existing student tab table")

    # 3. Add sqlite student sync to syncWithSQLiteDatabase
    if "/api/students" not in content:
        pos_sync = content.find("        // Fetch Quizzes from SQLite table 'quizzes'")
        if pos_sync != -1:
            content = content[:pos_sync] + sqlite_student_sync_code + "\n" + content[pos_sync:]
            print("  ✓ Added /api/students synchronization to syncWithSQLiteDatabase")

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)\n")

print("All files updated successfully!")

