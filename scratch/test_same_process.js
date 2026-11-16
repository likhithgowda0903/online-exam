const http = require('http');

const server = http.createServer((req, res) => {
  res.writeHead(200, { 'Content-Type': 'application/json' });
  res.end(JSON.stringify({ status: 'ok' }));
});

server.listen(5599, '127.0.0.1', () => {
  console.log('Listening on 127.0.0.1:5599');
  http.get('http://127.0.0.1:5599', (res) => {
    let d = '';
    res.on('data', c => d += c);
    res.on('end', () => {
      console.log('CLIENT RECEIVED:', d);
      server.close();
    });
  }).on('error', e => {
    console.error('ERROR:', e.message);
    server.close();
  });
});

