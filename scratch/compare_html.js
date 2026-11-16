const fs = require('fs');

const rootHtml = fs.readFileSync('index.html', 'utf8');
const wpHtml = fs.readFileSync('web-portal/index.html', 'utf8');

console.log('rootHtml length:', rootHtml.length);
console.log('wpHtml length:', wpHtml.length);

// Check if web-portal/index.html has the table alignment fixes from earlier tasks
console.log('web-portal has Official Course Quizzes & Modules:', wpHtml.includes('Official Course Quizzes & Modules'));
console.log('root has Official Course Quizzes & Modules:', rootHtml.includes('Official Course Quizzes & Modules'));

console.log('web-portal has Student Candidate Registry:', wpHtml.includes('Student Candidate Registry'));
console.log('root has Student Candidate Registry:', rootHtml.includes('Student Candidate Registry'));

console.log('web-portal has getAdminExaminations:', wpHtml.includes('getAdminExaminations'));
console.log('root has getAdminExaminations:', rootHtml.includes('getAdminExaminations'));

