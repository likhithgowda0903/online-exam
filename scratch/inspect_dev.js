const fs = require('fs');

console.log('--- Checking root dev-server.js ---');
const devServer = fs.readFileSync('dev-server.js', 'utf8');
console.log('dev-server handles /api:', devServer.includes('/api'));

console.log('--- Checking START_WEBSITE.bat ---');
const startBat = fs.readFileSync('START_WEBSITE.bat', 'utf8');
console.log(startBat);

