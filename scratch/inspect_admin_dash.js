const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf8');

const idx = html.indexOf('function renderAdminDashboard(');
console.log(html.substring(idx + 4000, idx + 8000));
