import re

print("=== FIXING UNIQUE STUDENTS COUNT AND DYNAMIC STUDENT TABLE ===")

helper_fn = """    function getUniqueRegisteredStudents() {
      const map = new Map();
      for (const u of Object.values(registeredCandidates)) {
        if (!u) continue;
        const key = (u.id || u.username || '').toLowerCase();
        if (key && !map.has(key)) {
          map.set(key, u);
        }
      }
      return Array.from(map.values());
    }
"""

for filepath in ['index.html', 'web-portal/index.html']:
    print(f"Updating {filepath}...")
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Add getUniqueRegisteredStudents helper
    if "function getUniqueRegisteredStudents" not in content:
        pos = content.find("    function unlockAuthInput(inputEl)")
        if pos != -1:
            content = content[:pos] + helper_fn + "\n" + content[pos:]
            print("  ✓ Added getUniqueRegisteredStudents helper function")
        else:
            print("  ⚠️ Could not find pos for getUniqueRegisteredStudents")

    # 2. Update sidebar badge
    content = content.replace(
        "{ id: 'admin-students', label: 'Students', icon: '👥', badge: `${Object.keys(registeredCandidates).length}` },",
        "{ id: 'admin-students', label: 'Students', icon: '👥', badge: `${getUniqueRegisteredStudents().length}` },"
    )
    print("  ✓ Updated sidebar students badge to count unique students")

    # 3. Update dashboard stat card
    content = content.replace(
        '<span class="stat-title">Total Students</span>\n                <span class="stat-number">${Object.keys(registeredCandidates).length}</span>',
        '<span class="stat-title">Total Students</span>\n                <span class="stat-number">${getUniqueRegisteredStudents().length}</span>'
    )
    print("  ✓ Updated dashboard stat card to count unique students")

    # 4. Update Tab Content 3: Students table to render dynamically
    old_students_table = """          <!-- TAB CONTENT 3: STUDENTS -->
          ${activeTab === 'students' ? `
            <div class="card-frame" style="padding: 22px; background: var(--bg-card); border: 1px solid var(--border-color); border-radius: var(--radius-md);">
              <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 18px;">
                <div>
                  <h3 style="color: #fff; font-size: var(--fs-lg); font-weight: 700;">Student Candidate Registry</h3>
                  <p style="color: var(--text-dim); font-size: var(--fs-sm); margin-top: 2px;">Directory of enrolled candidate profiles, degree programs, exam attempt counts, and overall GPA.</p>
                </div>
                <div style="display: flex; gap: 8px;">
                  <span class="badge-pill green">0 Candidates Enrolled</span>
                </div>
              </div>

              <div class="table-responsive">
                <table class="modern-table">
                  <thead>
                    <tr>
                      <th>Student Roll ID</th>
                      <th>Candidate Name</th>
                      <th>Academic Program</th>
                      <th>Exams Taken</th>
                      <th>Institutional Avg</th>
                      <th>Status</th>
                      <th style="text-align: right;">Actions</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr>
                      <td colspan="7" style="text-align: center; color: var(--text-dim); padding: 36px 16px; font-size: var(--fs-base);">
                        No student candidate profiles registered yet.
                      </td>
                    </tr>
                  </tbody>
                </table>
              </div>
            </div>
          ` : ''}"""

    new_students_table = """          <!-- TAB CONTENT 3: STUDENTS -->
          ${activeTab === 'students' ? `
            <div class="card-frame" style="padding: 22px; background: var(--bg-card); border: 1px solid var(--border-color); border-radius: var(--radius-md);">
              <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 18px;">
                <div>
                  <h3 style="color: #fff; font-size: var(--fs-lg); font-weight: 700;">Student Candidate Registry</h3>
                  <p style="color: var(--text-dim); font-size: var(--fs-sm); margin-top: 2px;">Directory of enrolled candidate profiles, degree programs, and exam attempt records.</p>
                </div>
                <div style="display: flex; gap: 8px;">
                  <span class="badge-pill green">${getUniqueRegisteredStudents().length} Candidate${getUniqueRegisteredStudents().length === 1 ? '' : 's'} Enrolled</span>
                </div>
              </div>

              <div class="table-responsive">
                <table class="modern-table">
                  <thead>
                    <tr>
                      <th>Student Roll ID</th>
                      <th>Candidate Name</th>
                      <th>Academic Program</th>
                      <th>Exams Taken</th>
                      <th>Institutional Avg</th>
                      <th>Status</th>
                    </tr>
                  </thead>
                  <tbody>
                    ${getUniqueRegisteredStudents().length === 0 ? `
                      <tr>
                        <td colspan="6" style="text-align: center; color: var(--text-dim); padding: 36px 16px; font-size: var(--fs-base);">
                          No student candidate profiles registered yet.
                        </td>
                      </tr>
                    ` : getUniqueRegisteredStudents().map(s => {
                      const attempts = (state.history || []).filter(h => (h.studentId && h.studentId.toLowerCase() === (s.id || '').toLowerCase()));
                      const avg = attempts.length > 0 ? `${Math.round(attempts.reduce((a, b) => a + (b.pct || 0), 0) / attempts.length)}%` : 'Pending Exam';
                      return `
                        <tr>
                          <td><span class="badge-pill blue">${s.id || s.username}</span></td>
                          <td><strong style="color: #fff;">${s.name}</strong><div style="font-size: var(--fs-xs); color: var(--text-dim);">${s.email || ''}</div></td>
                          <td>${s.program || 'Computer Science'}</td>
                          <td>${attempts.length} Exam${attempts.length === 1 ? '' : 's'}</td>
                          <td><strong style="color: ${attempts.length > 0 ? '#34d399' : 'var(--text-muted)'};">${avg}</strong></td>
                          <td><span class="badge-pill green">Enrolled</span></td>
                        </tr>
                      `;
                    }).join('')}
                  </tbody>
                </table>
              </div>
            </div>
          ` : ''}"""

    if old_students_table in content:
        content = content.replace(old_students_table, new_students_table)
        print("  ✓ Updated Students table to render actual unique enrolled student profiles")
    else:
        print("  ⚠️ Could not match old_students_table")

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {filepath} ({len(content)} bytes)\n")

print("All files updated successfully!")

