const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

[197403, 204916, 208321].forEach((pos, idx) => {
  console.log(`=== CALL ${idx + 1} at ${pos} ===`);
  console.log(html.substring(pos - 60, pos + 100));
});
