const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const idx = html.indexOf('function handleCreateQuizSubmit(');
console.log(html.substring(idx + 1500, idx + 2600));

