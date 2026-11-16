const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const funcs = ['deleteExam', 'deleteAllExams', 'switchAdminExamPool', 'handleAddQuestionSubmit', 'deleteQuestionFromPool'];

funcs.forEach(f => {
  const idx = html.indexOf('function ' + f);
  console.log('=== ' + f + ' ===');
  if (idx !== -1) {
    console.log(html.substring(idx, idx + 800));
  } else {
    console.log('NOT FOUND');
  }
});

