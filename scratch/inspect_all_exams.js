const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const regex = /allExams/g;
let match;
let count = 0;
while ((match = regex.exec(html)) !== null) {
  count++;
  if ([68, 69, 70].includes(count)) {
    console.log(`=== MATCH ${count} at ${match.index} ===`);
    console.log(html.substring(Math.max(0, match.index - 200), Math.min(html.length, match.index + 300)));
  }
}
