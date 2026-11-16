import json
import sys
import os

sys.path.append('scratch')
from seed_oop_exam import OOP_EXAM, OOP_QUESTIONS

# 1. Generate JS questions array
js_questions = []
for q in OOP_QUESTIONS:
    js_questions.append({
        "id": q["num"],
        "text": f"{q['num']}. {q['text']}",
        "options": q["options"],
        "correct": q["correct"],
        "marks": q["marks"],
        "explanation": q["explanation"]
    })

js_questions_str = json.dumps(js_questions, indent=6)

js_pools_definition = f"""    const defaultOOPQuestions = {js_questions_str};

    const masterQuestionPools = {{
      'OOP': defaultOOPQuestions
    }};
    const allExams = {{
      'OOP': {{
        id: 'OOP',
        title: 'Object-Oriented Programming (OOPs)',
        category: 'C++ Programming',
        totalQuestions: 20,
        durationMinutes: 15,
        passingPercentage: 60,
        totalMarks: 100,
        questions: []
      }}
    }};"""

# Update index.html and web-portal/index.html
target_files = ['index.html', 'web-portal/index.html']

for fpath in target_files:
    print(f"Updating {fpath}...")
    with open(fpath, 'r', encoding='utf-8') as f:
        content = f.read()

    # A. Replace empty definitions
    target_empty_def = """    const masterQuestionPools = {};\n    const allExams = {};"""
    if target_empty_def in content:
        content = content.replace(target_empty_def, js_pools_definition)
        print("  ✓ Replaced masterQuestionPools & allExams initial definition")
    else:
        print("  ⚠️ target_empty_def not found!")

    # B. Update SQLite sync callback to rotate exam questions and re-render if active
    target_sqlite_sync = """          if (sqlitePools && Object.keys(sqlitePools).length > 0) {
            Object.assign(masterQuestionPools, sqlitePools);
            saveQuestionPoolsToStorage();
          }"""
    replacement_sqlite_sync = """          if (sqlitePools && Object.keys(sqlitePools).length > 0) {
            Object.assign(masterQuestionPools, sqlitePools);
            saveQuestionPoolsToStorage();
            Object.keys(allExams).forEach(id => {
              rotateExamQuestions(id, true);
            });
            const app = document.getElementById('appViewport');
            if (app && state.isAuthenticated) {
              if (state.currentView === 'dashboard') {
                renderDashboard(app);
              } else if (state.currentView === 'admin' && state.adminActiveTab === 'questions') {
                renderAdminDashboard(app);
              }
            }
          }"""
    if target_sqlite_sync in content:
        content = content.replace(target_sqlite_sync, replacement_sqlite_sync)
        print("  ✓ Updated sqlite sync handler with dynamic exam rotation & render")
    else:
        print("  ⚠️ target_sqlite_sync not found!")

    # C. Update localStorage restore section to guarantee OOP is preserved and not deleted
    target_restore = """      // Apply permanently deleted exams list so deleted exams NEVER reappear on refresh
      const deletedExamIds = JSON.parse(localStorage.getItem('online_exam_deleted_exam_ids') || '["EX-101","EX-102","EX-103","EX-100"]');
      if (Array.isArray(deletedExamIds)) {
        deletedExamIds.forEach(id => {
          delete allExams[id];
          delete masterQuestionPools[id];
        });
      }
      // Ensure the 4 legacy demo subjects are expunged from localStorage permanently
      ['EX-101', 'EX-102', 'EX-103', 'EX-100'].forEach(id => {
        delete allExams[id];
        delete masterQuestionPools[id];
      });
      saveExamsToStorage();
      saveQuestionPoolsToStorage();"""

    replacement_restore = """      // Apply permanently deleted exams list so deleted exams NEVER reappear on refresh
      let deletedExamIds = [];
      try {
        deletedExamIds = JSON.parse(localStorage.getItem('online_exam_deleted_exam_ids') || '["EX-101","EX-102","EX-103","EX-100"]');
      } catch (e) { }
      if (Array.isArray(deletedExamIds)) {
        // Guarantee OOP is never pruned by legacy deleted list
        deletedExamIds = deletedExamIds.filter(id => id !== 'OOP');
        try { localStorage.setItem('online_exam_deleted_exam_ids', JSON.stringify(deletedExamIds)); } catch (e) { }
        deletedExamIds.forEach(id => {
          delete allExams[id];
          delete masterQuestionPools[id];
        });
      }
      // Ensure the 4 legacy demo subjects are expunged from localStorage permanently
      ['EX-101', 'EX-102', 'EX-103', 'EX-100'].forEach(id => {
        delete allExams[id];
        delete masterQuestionPools[id];
      });

      // Guarantee the OOP exam and its 20 questions are always populated
      if (!allExams['OOP'] || Object.keys(allExams).length === 0) {
        allExams['OOP'] = {
          id: 'OOP',
          title: 'Object-Oriented Programming (OOPs)',
          category: 'C++ Programming',
          totalQuestions: 20,
          durationMinutes: 15,
          passingPercentage: 60,
          totalMarks: 100,
          questions: []
        };
      }
      if (!masterQuestionPools['OOP'] || masterQuestionPools['OOP'].length === 0) {
        masterQuestionPools['OOP'] = [...defaultOOPQuestions];
      }
      saveExamsToStorage();
      saveQuestionPoolsToStorage();"""

    if target_restore in content:
        content = content.replace(target_restore, replacement_restore)
        print("  ✓ Updated localStorage restore block with OOP guarantee")
    else:
        print("  ⚠️ target_restore not found!")

    # D. Replace fallback 'EX-101' in openAddQuestionModal
    target_add_btn = "openAddQuestionModal(state.adminSelectedExam || 'EX-101')"
    if target_add_btn in content:
        content = content.replace(target_add_btn, "openAddQuestionModal(state.adminSelectedExam || 'OOP')")
        print("  ✓ Replaced fallback EX-101 with OOP in openAddQuestionModal")

    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Saved {fpath} ({len(content)} bytes)")

print("HTML files update complete!")

