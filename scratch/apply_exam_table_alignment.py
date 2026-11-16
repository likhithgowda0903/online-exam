import re

print("=== APPLYING EXAMINATION TABLE ALIGNMENT FIX ===")

css_addition = """    /* =========================================================================
       ADMINISTRATOR EXAMINATION MODULES TABLE STYLES
       Guarantees recommended column proportions, header-data alignment & responsive scroll
       ========================================================================= */
    .table-responsive-admin {
      width: 100%;
      overflow-x: auto;
      -webkit-overflow-scrolling: touch;
      border-radius: var(--radius-sm);
      margin-top: 8px;
    }

    .admin-exam-table {
      width: 100%;
      min-width: 960px;
      border-collapse: separate;
      border-spacing: 0;
      table-layout: fixed;
    }

    .admin-exam-table th {
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

    .admin-exam-table td {
      padding: 16px 12px;
      border-bottom: 1px solid rgba(255, 255, 255, 0.06);
      font-size: var(--fs-sm);
      color: #e2e8f0;
      vertical-align: middle;
      box-sizing: border-box;
    }

    .admin-exam-table tr:hover td {
      background: rgba(30, 58, 138, 0.14);
    }

    .admin-exam-col-title {
      width: 25%;
      text-align: left;
    }

    .admin-exam-col-category {
      width: 13%;
      text-align: left;
    }

    .admin-exam-col-questions {
      width: 12%;
      text-align: center;
    }

    .admin-exam-col-duration {
      width: 8%;
      text-align: center;
    }

    .admin-exam-col-pass {
      width: 7%;
      text-align: center;
    }

    .admin-exam-col-marks {
      width: 9%;
      text-align: center;
    }

    .admin-exam-col-actions {
      width: 26%;
      text-align: right;
    }

    .admin-action-btn-group {
      display: inline-flex;
      align-items: center;
      justify-content: flex-end;
      gap: 8px;
      flex-wrap: nowrap;
    }

    .admin-exam-btn {
      height: 32px;
      padding: 0 12px;
      font-size: var(--fs-xs);
      font-weight: 700;
      white-space: nowrap;
      border-radius: var(--radius-sm);
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 5px;
      cursor: pointer;
      box-sizing: border-box;
      transition: all 0.2s;
    }

    .admin-exam-btn:hover {
      transform: translateY(-1px);
    }
"""

new_table_html = """              <div class="table-responsive-admin">
                <table class="admin-exam-table">
                  <colgroup>
                    <col style="width: 25%;">
                    <col style="width: 13%;">
                    <col style="width: 12%;">
                    <col style="width: 8%;">
                    <col style="width: 7%;">
                    <col style="width: 9%;">
                    <col style="width: 26%;">
                  </colgroup>
                  <thead>
                    <tr>
                      <th class="admin-exam-col-title" style="text-align: left;">Exam Code & Title</th>
                      <th class="admin-exam-col-category" style="text-align: left;">Category</th>
                      <th class="admin-exam-col-questions" style="text-align: center;">Questions in Pool</th>
                      <th class="admin-exam-col-duration" style="text-align: center;">Duration</th>
                      <th class="admin-exam-col-pass" style="text-align: center;">Pass %</th>
                      <th class="admin-exam-col-marks" style="text-align: center;">Total Marks</th>
                      <th class="admin-exam-col-actions" style="text-align: right;">Administrative Actions</th>
                    </tr>
                  </thead>
                  <tbody>
                    ${Object.values(allExams).length === 0 ? `
                    <tr>
                      <td colspan="7" style="text-align: center; padding: 40px 20px; color: var(--text-muted);">
                        <div style="font-size: 2.2rem; margin-bottom: 10px;">📋</div>
                        <div style="font-size: 1.1rem; font-weight: 800; color: #fff;">No Examinations In System</div>
                        <div style="font-size: 0.85rem; margin-top: 6px; color: #94a3b8;">Click "➕ New Examination" above to configure a course exam module.</div>
                      </td>
                    </tr>
                  ` : Object.values(allExams).map(e => {
        const poolCount = masterQuestionPools[e.id] ? masterQuestionPools[e.id].length : (e.totalQuestions || 20);
        return `
                        <tr>
                          <td class="admin-exam-col-title" style="text-align: left;">
                            <div style="font-weight: 800; color: #fff; font-size: var(--fs-base); line-height: 1.35;">${e.title}</div>
                            <div style="font-size: var(--fs-xs); color: #60a5fa; margin-top: 3px; font-weight: 600;">Code: ${e.id} &bull; Master Pool Active</div>
                          </td>
                          <td class="admin-exam-col-category" style="text-align: left;">
                            <span class="badge-pill blue" style="font-weight: 700; font-size: var(--fs-xs);">${e.category || 'Core Computer Science'}</span>
                          </td>
                          <td class="admin-exam-col-questions" style="text-align: center;">
                            <strong style="color: #fff; font-size: var(--fs-base);">${poolCount}</strong> <span style="color: var(--text-muted); font-size: var(--fs-xs);">MCQs</span>
                          </td>
                          <td class="admin-exam-col-duration" style="text-align: center;">
                            <span style="color: #fff; font-weight: 700;">${e.durationMinutes || 15}</span> <span style="color: var(--text-muted); font-size: var(--fs-xs);">mins</span>
                          </td>
                          <td class="admin-exam-col-pass" style="text-align: center;">
                            <strong style="color: #34d399; font-size: var(--fs-base);">${e.passingPercentage || 60}%</strong>
                          </td>
                          <td class="admin-exam-col-marks" style="text-align: center;">
                            <strong style="color: #cbd5e1; font-size: var(--fs-base);">100</strong> <span style="color: var(--text-muted); font-size: var(--fs-xs);">pts</span>
                          </td>
                          <td class="admin-exam-col-actions" style="text-align: right;">
                            <div class="admin-action-btn-group">
                              <button class="btn-action-outline admin-exam-btn" style="border-color: #8b5cf6; color: #c4b5fd;" onclick="switchAdminExamPool('${e.id}'); switchAdminTab('questions');" title="Manage Questions for ${e.id}">
                                <span>❓</span> Manage Questions
                              </button>
                              <button class="btn-action-outline admin-exam-btn" style="border-color: #3b82f6; color: #93c5fd;" onclick="state.selectedExamId = '${e.id}'; showView('exam-room');" title="Preview Examination">
                                <span>▶</span> Preview
                              </button>
                              <button class="btn-action-outline admin-exam-btn" style="border-color: #ef4444; color: #fca5a5;" onclick="deleteExam('${e.id}')" title="Delete Examination">
                                <span>🗑️</span> Delete Exam
                              </button>
                            </div>
                          </td>
                        </tr>
                      `;
      }).join('')}
                  </tbody>
                </table>
              </div>"""

for fpath in ['index.html', 'web-portal/index.html']:
    print(f"Updating {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Add CSS
    if ".table-responsive-admin" not in content:
        pos_table_css = content.find("    /* TABLES & CONTROLS */")
        if pos_table_css != -1:
            content = content[:pos_table_css] + css_addition + "\n" + content[pos_table_css:]
            print("  ✓ Added dedicated admin examination table CSS")
        else:
            print("  ⚠️ Could not find insert pos for table CSS")

    # 2. Replace the quizzes tab table
    # Match from <div class="table-responsive">\n                <table class="modern-table"> to </table>\n              </div>
    pattern = re.compile(
        r'<div class="table-responsive">\s*<table class="modern-table">\s*<thead>\s*<tr>\s*<th>Exam Code & Title</th>[\s\S]*?</table>\s*</div>',
        re.MULTILINE
    )
    if pattern.search(content):
        content = pattern.sub(new_table_html, content, count=1)
        print("  ✓ Replaced quizzes tab table with perfectly aligned structure")
    else:
        print("  ⚠️ Could not regex match existing quizzes table")

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)\n")

print("All files updated successfully!")

