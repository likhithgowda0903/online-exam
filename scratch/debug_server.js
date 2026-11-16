const http = require('http');
const { spawn } = require('child_process');

async function test() {
  const serverProc = spawn('node', ['dev-server.js'], {
    env: { ...process.env, PORT: '5599' }
  });

  serverProc.stdout.on('data', d => console.log('SERVER STDOUT:', d.toString()));
  serverProc.stderr.on('data', d => console.log('SERVER STDERR:', d.toString()));

  await new Promise(r => setTimeout(r, 1500));

  http.get('http://localhost:5599/api/status', (res) => {
    let d = '';
    res.on('data', c => d += c);
    res.on('end', () => {
      console.log('STATUS RES:', res.statusCode, d);
      serverProc.kill();
    });
  }).on('error', e => {
    console.error('HTTP ERROR:', e);
    serverProc.kill();
  });
}

test();

