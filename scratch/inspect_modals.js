const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf8');

const funcs = [
  'openAdminForgotPassword',
  'openStudentForgotPassword',
  'openCreateQuizModal',
  'openAddQuestionModal',
  'openDisqualificationModal',
  'openScorecardModal',
  'openDatabaseInstructionModal',
  'openNotificationsModal'
];

funcs.forEach(f => {
  const idx = html.indexOf('function ' + f);
  if (idx !== -1) {
    console.log('=== ' + f + ' ===');
    console.log(html.substring(idx, idx + 350).replace(/\n/g, ' '));
  }
});
