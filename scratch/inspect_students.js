const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const sIdx = html.indexOf('TAB CONTENT 3');
if (sIdx !== -1) {
  console.log(html.substring(sIdx - 50, sIdx + 1500));
} else {
  const s2 = html.indexOf("activeTab === 'students' ? `");
  console.log('s2 at:', s2);
  if (s2 !== -1) console.log(html.substring(s2, s2 + 1500));
}
