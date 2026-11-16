const fs = require('fs');
const html = fs.readFileSync('web-portal/index.html', 'utf8');

const sIdx = html.indexOf('const state =');
const s2Idx = html.indexOf('let state =');
const s3Idx = html.indexOf('var state =');
console.log('const state:', sIdx, 'let state:', s2Idx, 'var state:', s3Idx);

