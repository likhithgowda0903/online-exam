const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf8');

const target = 'online_exam_active_user';
let pos = 0;
while (true) {
  const idx = html.indexOf(target, pos);
  if (idx === -1) break;
  console.log('Match at', idx, ':', html.substring(idx - 60, idx + 140).replace(/\n/g, ' '));
  pos = idx + target.length;
}

