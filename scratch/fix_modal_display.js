const fs = require('fs');

function fixModalDisplay(filePath) {
  let content = fs.readFileSync(filePath, 'utf8');

  // Fix openCreateQuizModal
  const oldCreate = `function openCreateQuizModal() {
      const modal = document.getElementById('modalRoot');
      modal.innerHTML = \``;
  const newCreate = `function openCreateQuizModal() {
      const modal = document.getElementById('modalRoot');
      if (modal) modal.style.display = 'block';
      modal.innerHTML = \``;

  // Fix openAddQuestionModal
  const oldAddQ = `function openAddQuestionModal(preselectedExam) {
      const modal = document.getElementById('modalRoot');
      const adminExams = getAdminExaminations();
      if (adminExams.length === 0) {
        showToastNotification("⚠️ Please create an examination module first before adding questions.");
        openCreateQuizModal();
        return;
      }`;
  const newAddQ = `function openAddQuestionModal(preselectedExam) {
      const modal = document.getElementById('modalRoot');
      const adminExams = getAdminExaminations();
      if (adminExams.length === 0) {
        showToastNotification("⚠️ Please create an examination module first before adding questions.");
        openCreateQuizModal();
        return;
      }
      if (modal) modal.style.display = 'block';`;

  // Fix openDisqualificationModal
  const oldDisqual = `function openDisqualificationModal(examTitle, examId, attemptNum = 1) {
      const modal = document.getElementById('modalRoot');
      const violationLogs = (state.proctorLogs || []).filter(l => l.level === 'warning' || l.level === 'danger');

      modal.innerHTML = \``;
  const newDisqual = `function openDisqualificationModal(examTitle, examId, attemptNum = 1) {
      const modal = document.getElementById('modalRoot');
      if (modal) modal.style.display = 'block';
      const violationLogs = (state.proctorLogs || []).filter(l => l.level === 'warning' || l.level === 'danger');

      modal.innerHTML = \``;

  // Fix openScorecardModal
  const oldScorecard = `function openScorecardModal(examTitle, examId, score = 18, total = 20, pct = 90, attemptNum = 1) {
      const modal = document.getElementById('modalRoot');
      const isPassed = pct >= 60;`;
  const newScorecard = `function openScorecardModal(examTitle, examId, score = 18, total = 20, pct = 90, attemptNum = 1) {
      const modal = document.getElementById('modalRoot');
      if (modal) modal.style.display = 'block';
      const isPassed = pct >= 60;`;

  // Fix openDatabaseInstructionModal
  const oldDbModal = `function openDatabaseInstructionModal(data) {
      const modal = document.getElementById('modalRoot');
      const dbFile = data.database_file || 'online_exam.db';`;
  const newDbModal = `function openDatabaseInstructionModal(data) {
      const modal = document.getElementById('modalRoot');
      if (modal) modal.style.display = 'block';
      const dbFile = data.database_file || 'online_exam.db';`;

  // Fix openNotificationsModal
  const oldNotifModal = `function openNotificationsModal() {
      const modal = document.getElementById('modalRoot');
      modal.innerHTML = \``;
  const newNotifModal = `function openNotificationsModal() {
      const modal = document.getElementById('modalRoot');
      if (modal) modal.style.display = 'block';
      modal.innerHTML = \``;

  // Fix closeModal to also clear innerHTML
  const oldClose = `function closeModal() {
      const modal = document.getElementById('modalRoot');
      if (modal) modal.style.display = 'none';
      stopProctoringSystem();
    }`;
  const newClose = `function closeModal() {
      const modal = document.getElementById('modalRoot');
      if (modal) {
        modal.style.display = 'none';
        modal.innerHTML = '';
      }
      stopProctoringSystem();
    }`;

  const hasCRLF = content.includes('\r\n');
  let normContent = content.replace(/\r\n/g, '\n');

  const replacements = [
    [oldCreate, newCreate, 'openCreateQuizModal display:block'],
    [oldAddQ, newAddQ, 'openAddQuestionModal display:block'],
    [oldDisqual, newDisqual, 'openDisqualificationModal display:block'],
    [oldScorecard, newScorecard, 'openScorecardModal display:block'],
    [oldDbModal, newDbModal, 'openDatabaseInstructionModal display:block'],
    [oldNotifModal, newNotifModal, 'openNotificationsModal display:block'],
    [oldClose, newClose, 'closeModal clean innerHTML']
  ];

  let count = 0;
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
  console.log(`Updated ${count}/${replacements.length} modal fixes in ${filePath}.\n`);
}

fixModalDisplay('index.html');
fixModalDisplay('web-portal/index.html');

