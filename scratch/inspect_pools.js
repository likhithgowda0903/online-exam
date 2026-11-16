const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf8');

const idx = html.indexOf('const qRes = await fetch(\'/api/questions\');');
console.log(html.substring(idx - 100, idx + 800));
