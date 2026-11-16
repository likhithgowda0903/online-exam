const fs = require('fs');
const cpp = fs.readFileSync('cpp-backend/server.cpp', 'utf8');

const idx = cpp.indexOf('saveExam(');
console.log(cpp.substring(idx + 1000, idx + 2500));
