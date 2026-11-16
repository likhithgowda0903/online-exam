const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const loginIdx = html.indexOf('function loginUser(userData) {');
console.log(html.substring(loginIdx, loginIdx + 200));

const outIdx = html.indexOf('function handleSignOut() {');
console.log(html.substring(outIdx, outIdx + 200));
