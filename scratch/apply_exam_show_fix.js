const fs = require('fs');

function applyFixes(filePath) {
  let content = fs.readFileSync(filePath, 'utf8');

  // 1. syncWithSQLiteDatabase
  const oldSync = `sqliteExams.forEach(e => {
              allExams[e.id] = {
                ...(allExams[e.id] || {}),
                ...e
              };
            });`;
  const newSync = `sqliteExams.forEach(e => {
              const existingCreator = allExams[e.id] ? (allExams[e.id].created_by || allExams[e.id].admin_id) : null;
              allExams[e.id] = {
                ...(allExams[e.id] || {}),
                ...e,
                created_by: e.created_by || existingCreator || null
              };
            });`;

  // 2. getAdminExaminations
  const oldGetExams = `function getAdminExaminations() {
      if (!state.user || state.user.role !== 'admin') {
        return Object.values(allExams);
      }
      const currentAdminId = (state.user.id || state.user.username || '').toLowerCase();
      const currentAdminUser = (state.user.username || '').toLowerCase();

      return Object.values(allExams).filter(exam => {
        if (!exam) return false;
        const creator = (exam.created_by || exam.creator_id || '').toLowerCase();
        if (!creator) {
          // Requirement 7: Existing examinations without an assigned owner
          // are handled safely and not automatically assigned to any administrator.
          return false;
        }
        return creator === currentAdminId || creator === currentAdminUser;
      });
    }`;
  const newGetExams = `function getAdminExaminations() {
      if (!state.user || state.user.role !== 'admin') {
        return Object.values(allExams);
      }
      const currentAdminId = (state.user.id || state.user.username || '').toLowerCase();
      const currentAdminUser = (state.user.username || '').toLowerCase();
      const currentAdminEmail = (state.user.email || '').toLowerCase();

      return Object.values(allExams).filter(exam => {
        if (!exam) return false;
        const creator = (exam.created_by || exam.creator_id || exam.admin_id || '').toLowerCase();
        if (!creator) {
          // Requirement 7: Existing examinations without an assigned owner
          // are handled safely and not automatically assigned to any administrator.
          return false;
        }
        return creator === currentAdminId ||
               creator === currentAdminUser ||
               (currentAdminEmail && creator === currentAdminEmail);
      });
    }`;

  // 3. handleCreateQuizSubmit - set adminActiveTab to 'quizzes' and add admin_id
  const oldCreateEnd = `state.adminSelectedExam = code;
      closeModal();
      showToastNotification(\`✓ Successfully created examination module: \${title} (\${code})!\`);`;
  const newCreateEnd = `state.adminActiveTab = 'quizzes';
      state.adminSelectedExam = code;
      closeModal();
      showToastNotification(\`✓ Successfully created examination module: \${title} (\${code})!\`);`;

  // 4. allExams[code] in handleCreateQuizSubmit
  const oldAllExamsCreate = `allExams[code] = {
        id: code,
        title: title,
        category: cat,
        totalQuestions: 0,
        durationMinutes: dur,
        passingPercentage: pass,
        totalMarks: marksVal,
        created_by: creatorId,
        questions: []
      };`;
  const newAllExamsCreate = `allExams[code] = {
        id: code,
        title: title,
        category: cat,
        totalQuestions: 0,
        durationMinutes: dur,
        passingPercentage: pass,
        totalMarks: marksVal,
        created_by: creatorId,
        admin_id: creatorId,
        questions: []
      };`;

  // 5. Course pill title truncation
  const oldPill = `<span style="font-weight: 800;">\${eid}</span>: \${exam.title ? exam.title.split(' ')[0] : eid}`;
  const newPill = `<span style="font-weight: 800;">\${eid}</span>: \${exam.title || eid}`;

  // 6. Question bank empty message
  const oldEmptyMsg = `<h4 style="color: #fff; font-size: var(--fs-lg); font-weight: 700; margin: 0;">No questions found</h4>
                  <p style="color: var(--text-dim); font-size: var(--fs-base); margin-top: 6px;">
                    Add questions to build your question bank.
                  </p>`;
  const newEmptyMsg = `<h4 style="color: #fff; font-size: var(--fs-lg); font-weight: 700; margin: 0;">\${selectedExam ? \`No questions in \${currentExamObj.title || selectedExam} (\${selectedExam}) yet\` : 'No questions found'}</h4>
                  <p style="color: var(--text-dim); font-size: var(--fs-base); margin-top: 6px;">
                    \${selectedExam ? \`Your examination module <strong>\${currentExamObj.title || selectedExam}</strong> is active in your registry. Click below to add questions to this exam pool.\` : 'Select or create an examination module to build your question bank.'}
                  </p>`;

  // 7. Legacy demo cleanup safe check
  const oldLegacyPurge = `// Ensure the 4 legacy demo subjects are expunged from localStorage permanently
      ['EX-101', 'EX-102', 'EX-103', 'EX-100'].forEach(id => {
        delete allExams[id];
        delete masterQuestionPools[id];
      });`;
  const newLegacyPurge = `// Ensure the 4 legacy demo subjects are expunged from localStorage permanently IF unowned
      ['EX-101', 'EX-102', 'EX-103', 'EX-100'].forEach(id => {
        if (allExams[id] && !allExams[id].created_by) {
          delete allExams[id];
          delete masterQuestionPools[id];
        }
      });`;

  // Normalize CRLF to LF for matching
  const hasCRLF = content.includes('\r\n');
  let normContent = content.replace(/\r\n/g, '\n');

  let count = 0;
  const replacements = [
    [oldSync, newSync, 'syncWithSQLiteDatabase'],
    [oldGetExams, newGetExams, 'getAdminExaminations'],
    [oldCreateEnd, newCreateEnd, 'handleCreateQuizSubmit tab switch'],
    [oldAllExamsCreate, newAllExamsCreate, 'handleCreateQuizSubmit admin_id'],
    [oldPill, newPill, 'course pill title'],
    [oldEmptyMsg, newEmptyMsg, 'question bank empty message'],
    [oldLegacyPurge, newLegacyPurge, 'legacy demo cleanup']
  ];

  for (const [target, replacement, desc] of replacements) {
    const normTarget = target.replace(/\r\n/g, '\n');
    const normReplacement = replacement.replace(/\r\n/g, '\n');
    if (normContent.includes(normTarget)) {
      normContent = normContent.replace(normTarget, normReplacement);
      console.log(`[OK] Replaced in ${filePath}: ${desc}`);
      count++;
    } else {
      console.warn(`[WARN] Target NOT found in ${filePath}: ${desc}`);
    }
  }

  if (hasCRLF) {
    normContent = normContent.replace(/\n/g, '\r\n');
  }

  fs.writeFileSync(filePath, normContent, 'utf8');
  console.log(`Applied ${count}/${replacements.length} fixes to ${filePath}.\n`);
}

applyFixes('index.html');
applyFixes('web-portal/index.html');

