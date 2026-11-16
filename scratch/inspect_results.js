const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

console.log(html.substring(225614 + 1200, 225614 + 2500));
