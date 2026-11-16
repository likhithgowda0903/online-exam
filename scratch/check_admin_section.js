const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const adminDashIdx = html.indexOf('function renderAdminDashboard(');
const adminDashEnd = html.indexOf('function renderDashboard(', adminDashIdx);

const adminSection = html.substring(adminDashIdx, adminDashEnd);

console.log('adminSection length:', adminSection.length);

const oopMatches = adminSection.match(/OOP/g) || [];
console.log('OOP matches in renderAdminDashboard:', oopMatches.length);

const allExamsMatches = adminSection.match(/allExams/g) || [];
console.log('allExams matches in renderAdminDashboard:', allExamsMatches.length);
let m;
const re = /allExams/g;
while ((m = re.exec(adminSection)) !== null) {
  console.log('allExams at pos ' + m.index + ':', adminSection.substring(Math.max(0, m.index - 30), Math.min(adminSection.length, m.index + 50)).replace(/\n/g, ' '));
}

