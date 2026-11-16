const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf8');

const idx = html.indexOf('// Guarantee the OOP exam');
console.log(html.substring(idx - 200, idx + 1200));

