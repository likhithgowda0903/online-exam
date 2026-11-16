const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const idx = html.indexOf('async function syncWithSQLiteDatabase()');
console.log(html.substring(idx, idx + 1800));

