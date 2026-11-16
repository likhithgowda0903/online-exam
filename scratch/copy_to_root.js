const fs = require('fs');

const src = 'web-portal/index.html';
const dest = 'index.html';

const content = fs.readFileSync(src, 'utf8');
fs.writeFileSync(dest, content, 'utf8');

const destContent = fs.readFileSync(dest, 'utf8');
console.log('Copied successfully!');
console.log('Source bytes:', Buffer.byteLength(content));
console.log('Dest bytes:', Buffer.byteLength(destContent));
console.log('Is identical:', content === destContent);

