/**
 * ============================================================================
 * ONLINE EXAMINATION SYSTEM - NODE.JS LOCAL DEVELOPMENT SERVER
 * ============================================================================
 * Zero-dependency local development server that serves the examination portal
 * with proper MIME types, CORS support, automatic browser opening, AND
 * real-time SQLite 3 database API synchronization (via sqlite_bridge.py).
 * ============================================================================
 */

const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');
const { exec, spawn } = require('child_process');

const PORT = process.env.PORT || 5500;
const ROOT_DIR = __dirname;
const PYTHON_CMD = process.platform === 'win32' ? 'python' : 'python3';
const BRIDGE_SCRIPT = path.join(ROOT_DIR, 'sqlite_bridge.py');

const MIME_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.htm': 'text/html; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.js': 'application/javascript; charset=utf-8',
  '.mjs': 'application/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.jpeg': 'image/jpeg',
  '.gif': 'image/gif',
  '.svg': 'image/svg+xml',
  '.ico': 'image/x-icon',
  '.mp3': 'audio/mpeg',
  '.wav': 'audio/wav',
  '.pdf': 'application/pdf'
};

function runBridge(args, inputData = null) {
  return new Promise((resolve, reject) => {
    const proc = spawn(PYTHON_CMD, [BRIDGE_SCRIPT, ...args], { cwd: ROOT_DIR });
    let stdout = '';
    let stderr = '';

    proc.stdout.on('data', (d) => { stdout += d.toString(); });
    proc.stderr.on('data', (d) => { stderr += d.toString(); });

    proc.on('close', (code) => {
      if (code !== 0 && !stdout) {
        return reject(new Error(stderr || `Process exited with code ${code}`));
      }
      try {
        const json = JSON.parse(stdout);
        resolve(json);
      } catch (err) {
        resolve({ raw: stdout });
      }
    });

    if (inputData) {
      proc.stdin.write(typeof inputData === 'string' ? inputData : JSON.stringify(inputData));
    }
    proc.stdin.end();
  });
}

const server = http.createServer(async (req, res) => {
  const parsedUrl = url.parse(req.url, true);
  let reqPath = decodeURI(parsedUrl.pathname);

  // CORS headers helper
  const setCorsHeaders = () => {
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization, X-Requested-With');
  };

  // Handle CORS preflight
  if (req.method === 'OPTIONS') {
    res.writeHead(204, {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type, Authorization, X-Requested-With',
      'Access-Control-Max-Age': '86400'
    });
    res.end();
    return;
  }

  // =========================================================================
  // API ROUTING (/api/*) WITH SQLITE 3 BRIDGE
  // =========================================================================
  if (reqPath.startsWith('/api/')) {
    setCorsHeaders();
    res.setHeader('Content-Type', 'application/json; charset=utf-8');

    // Helper to read JSON request body
    const getBody = () => new Promise((resolve) => {
      let data = '';
      req.on('data', chunk => { data += chunk; });
      req.on('end', () => {
        try {
          resolve(data ? JSON.parse(data) : {});
        } catch (e) {
          resolve({});
        }
      });
    });

    try {
      if (reqPath === '/api/status') {
        const status = await runBridge(['status']);
        res.writeHead(200);
        res.end(JSON.stringify(status));
        return;
      }

      if (reqPath === '/api/students') {
        const students = await runBridge(['students']);
        res.writeHead(200);
        res.end(JSON.stringify(students));
        return;
      }

      if (reqPath === '/api/admins') {
        const admins = await runBridge(['admins']);
        res.writeHead(200);
        res.end(JSON.stringify(admins));
        return;
      }

      if (reqPath === '/api/register' && req.method === 'POST') {
        const body = await getBody();
        const result = await runBridge(['register'], body);
        res.writeHead(200);
        res.end(JSON.stringify(result));
        return;
      }

      if (reqPath === '/api/exams') {
        const adminId = parsedUrl.query.admin_id || '';

        if (req.method === 'GET') {
          const args = adminId ? ['exams', adminId] : ['exams'];
          const exams = await runBridge(args);
          res.writeHead(200);
          res.end(JSON.stringify(exams));
          return;
        }

        if (req.method === 'POST') {
          const body = await getBody();
          const creator = body.created_by || body.admin_id || adminId || '';
          const result = await runBridge(['save_exam', creator], body);
          if (result.error && result.status_code === 403) {
            res.writeHead(403);
          } else {
            res.writeHead(200);
          }
          res.end(JSON.stringify(result));
          return;
        }

        if (req.method === 'DELETE') {
          const examId = parsedUrl.query.id || '';
          const result = await runBridge(['delete_exam', examId, adminId]);
          if (result.error && result.status_code === 403) {
            res.writeHead(403);
          } else {
            res.writeHead(200);
          }
          res.end(JSON.stringify(result));
          return;
        }
      }

      if (reqPath === '/api/exams/delete' && req.method === 'POST') {
        const body = await getBody();
        const examId = body.id || '';
        const adminId = body.admin_id || '';
        const result = await runBridge(['delete_exam', examId, adminId]);
        if (result.error && result.status_code === 403) {
          res.writeHead(403);
        } else {
          res.writeHead(200);
        }
        res.end(JSON.stringify(result));
        return;
      }

      if (reqPath === '/api/questions') {
        const quizId = parsedUrl.query.quiz_id || parsedUrl.query.exam_id || '';

        if (req.method === 'GET') {
          const args = quizId ? ['questions', quizId] : ['questions'];
          const questions = await runBridge(args);
          res.writeHead(200);
          res.end(JSON.stringify(questions));
          return;
        }

        if (req.method === 'POST') {
          const body = await getBody();
          const result = await runBridge(['save_question'], body);
          res.writeHead(200);
          res.end(JSON.stringify(result));
          return;
        }

        if (req.method === 'DELETE') {
          const qId = parsedUrl.query.id || '';
          const qQuizId = parsedUrl.query.quiz_id || '';
          const result = await runBridge(['delete_question', qId, qQuizId]);
          res.writeHead(200);
          res.end(JSON.stringify(result));
          return;
        }
      }

      // Default API 404
      res.writeHead(404);
      res.end(JSON.stringify({ error: 'Endpoint not found' }));
      return;
    } catch (err) {
      console.error('API Error:', err.message);
      res.writeHead(500);
      res.end(JSON.stringify({ error: err.message }));
      return;
    }
  }

  // =========================================================================
  // STATIC FILE SERVING
  // =========================================================================
  if (reqPath === '/' || reqPath === '') {
    reqPath = '/index.html';
  }

  let filePath = path.join(ROOT_DIR, reqPath);

  // If file doesn't exist, try fallback to index.html
  if (!fs.existsSync(filePath) || fs.statSync(filePath).isDirectory()) {
    filePath = path.join(ROOT_DIR, 'index.html');
  }

  if (fs.existsSync(filePath)) {
    const ext = path.extname(filePath).toLowerCase();
    const contentType = MIME_TYPES[ext] || 'application/octet-stream';

    res.writeHead(200, {
      'Content-Type': contentType,
      'Access-Control-Allow-Origin': '*',
      'Cache-Control': 'no-cache, no-store, must-revalidate'
    });

    const stream = fs.createReadStream(filePath);
    stream.pipe(res);
  } else {
    res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('404 Not Found');
  }
});

server.listen(PORT, () => {
  const url = `http://localhost:${PORT}/index.html`;

  console.log('============================================================');
  console.log('  ONLINE EXAMINATION SYSTEM - DEVELOPMENT SERVER           ');
  console.log('============================================================');
  console.log(`\n  Local Dev Server:  ${url}`);
  console.log(`  Serving files from: ${ROOT_DIR}`);
  console.log('\n  Features:');
  console.log('   - Full Administrator-Wise Exam Isolation & Ownership');
  console.log('   - SQLite 3 Live Database API Bridge (online_exam.db)');
  console.log('   - Real-time Webcam AI Proctoring & Candidate Directory');
  console.log('   - Instant Hot-Reload & Clean Session Authentication');
  console.log('\n  Press Ctrl+C to terminate the server.\n');
  console.log('============================================================\n');

  // Launch in user's default browser
  const openCmd = process.platform === 'win32' ? `start "" "${url}"` : `open "${url}"`;
  exec(openCmd, (err) => {
    if (!err) {
      console.log('✓ Successfully opened browser.');
    }
  });
});
