const fs = require('fs');
const cpp = fs.readFileSync('cpp-backend/server.cpp', 'utf8');

console.log(cpp.substring(37650, 38400));
