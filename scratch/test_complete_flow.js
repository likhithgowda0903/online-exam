const http = require('http');
const { spawn } = require('child_process');
const fs = require('fs');

async function run() {
  console.log('====================================================');
  console.log('END-TO-END TEST: ADMINISTRATOR-WISE EXAM ISOLATION');
  console.log('====================================================\n');

  // 1. Start server on test port 5599
  const serverProc = spawn('node', ['dev-server.js'], {
    env: { ...process.env, PORT: '5599' },
    stdio: 'pipe'
  });

  // Give server 1 second to start
  await new Promise(r => setTimeout(r, 1200));

  function request(path, method = 'GET', body = null) {
    return new Promise((resolve, reject) => {
      const postData = body ? JSON.stringify(body) : '';
      const req = http.request({
        hostname: 'localhost',
        port: 5599,
        path: path,
        method: method,
        headers: {
          'Content-Type': 'application/json',
          ...(postData ? { 'Content-Length': Buffer.byteLength(postData) } : {})
        }
      }, (res) => {
        let data = '';
        res.on('data', chunk => data += chunk);
        res.on('end', () => {
          try {
            resolve({ statusCode: res.statusCode, data: JSON.parse(data) });
          } catch (e) {
            resolve({ statusCode: res.statusCode, data });
          }
        });
      });
      req.on('error', reject);
      if (postData) req.write(postData);
      req.end();
    });
  }

  try {
    // Step 1: Check status
    console.log('1. Checking Server & SQLite status...');
    const status = await request('/api/status');
    console.log('   Status:', status.statusCode, status.data);
    if (status.statusCode !== 200 || status.data.status !== 'online') {
      throw new Error('Status failed');
    }

    // Step 2: Register Admin Alice (ADM101)
    console.log('\n2. Registering Admin Alice (ADM101)...');
    const regAlice = await request('/api/register', 'POST', {
      username: 'alice_admin',
      password: 'password123',
      id: 'ADM101',
      name: 'Alice Johnson',
      role: 'admin'
    });
    console.log('   Alice registration:', regAlice.data);

    // Step 3: Admin Alice checks exams before creating any
    console.log('\n3. Admin Alice checks her exams before creation...');
    const aliceExams0 = await request('/api/exams?admin_id=ADM101');
    console.log(`   Alice exams count: ${aliceExams0.data.length}`);
    if (aliceExams0.data.length !== 0) {
      throw new Error(`Expected 0 exams for fresh Alice, got ${aliceExams0.data.length}`);
    }
    console.log('   ✓ PASS: Fresh Admin Alice starts with exactly 0 exams.');

    // Step 4: Admin Alice creates exam CS101
    console.log('\n4. Admin Alice creates examination CS101...');
    const createAlice = await request('/api/exams', 'POST', {
      id: 'CS101',
      title: 'Multimedia & Web Technologies',
      category: 'Web Tech',
      durationMinutes: 45,
      passingPercentage: 60,
      totalMarks: 100,
      created_by: 'ADM101',
      admin_id: 'ADM101'
    });
    console.log('   Alice exam creation result:', createAlice.data);

    // Step 5: Verify Alice sees CS101
    console.log('\n5. Checking Alice exams after creation...');
    const aliceExams1 = await request('/api/exams?admin_id=ADM101');
    console.log('   Alice exams:', aliceExams1.data.map(e => e.id));
    if (aliceExams1.data.length !== 1 || aliceExams1.data[0].id !== 'CS101') {
      throw new Error('Alice does not see her exam CS101');
    }
    console.log('   ✓ PASS: Alice sees only her exam CS101.');

    // Step 6: Register Admin Bob (ADM202)
    console.log('\n6. Registering Admin Bob (ADM202)...');
    const regBob = await request('/api/register', 'POST', {
      username: 'bob_admin',
      password: 'password123',
      id: 'ADM202',
      name: 'Bob Miller',
      role: 'admin'
    });
    console.log('   Bob registration:', regBob.data);

    // Step 7: Verify Admin Bob does NOT see Alice exams!
    console.log('\n7. Checking Admin Bob exams (ISOLATION CHECK)...');
    const bobExams0 = await request('/api/exams?admin_id=ADM202');
    console.log(`   Bob exams count: ${bobExams0.data.length}`);
    if (bobExams0.data.length !== 0) {
      throw new Error(`FAIL: Bob saw exams: ${JSON.stringify(bobExams0.data)}`);
    }
    console.log('   ✓ PASS: Admin Bob does NOT see Admin Alice exam CS101!');

    // Step 8: Admin Bob creates exam CS202
    console.log('\n8. Admin Bob creates examination CS202...');
    const createBob = await request('/api/exams', 'POST', {
      id: 'CS202',
      title: 'Data Structures & Algorithms',
      category: 'Computer Science',
      durationMinutes: 60,
      passingPercentage: 70,
      totalMarks: 100,
      created_by: 'ADM202',
      admin_id: 'ADM202'
    });
    console.log('   Bob exam creation result:', createBob.data);

    // Step 9: Verify Bob sees ONLY CS202
    console.log('\n9. Checking Bob exams after creating CS202...');
    const bobExams1 = await request('/api/exams?admin_id=ADM202');
    console.log('   Bob exams:', bobExams1.data.map(e => e.id));
    if (bobExams1.data.length !== 1 || bobExams1.data[0].id !== 'CS202') {
      throw new Error('Bob does not see CS202');
    }
    console.log('   ✓ PASS: Bob sees ONLY his exam CS202.');

    // Step 10: Verify Alice STILL sees ONLY CS101 (NOT Bob's CS202)
    console.log('\n10. Checking Alice exams after Bob created CS202 (CROSS-CHECK)...');
    const aliceExams2 = await request('/api/exams?admin_id=ADM101');
    console.log('   Alice exams:', aliceExams2.data.map(e => e.id));
    if (aliceExams2.data.length !== 1 || aliceExams2.data[0].id !== 'CS101') {
      throw new Error('Alice saw exams other than CS101');
    }
    console.log('   ✓ PASS: Alice STILL sees ONLY CS101, completely isolated from Bob.');

    // Step 11: Security check - Bob tries to edit or delete Alice's CS101
    console.log('\n11. Bob attempts unauthorized modification of Alice\'s CS101...');
    const hackAttempt = await request('/api/exams?id=CS101&admin_id=ADM202', 'DELETE');
    console.log(`   Unauthorized delete response: HTTP ${hackAttempt.statusCode}`, hackAttempt.data);
    if (hackAttempt.statusCode !== 403) {
      throw new Error(`Expected 403 Forbidden, got ${hackAttempt.statusCode}`);
    }
    console.log('   ✓ PASS: Unauthorized cross-administrator modification correctly blocked with 403 Forbidden.');

    // Step 12: Student perspective - verify student sees all published exams
    console.log('\n12. Student candidate queries all published exams...');
    const studentExams = await request('/api/exams');
    const examIds = studentExams.data.map(e => e.id);
    console.log('   Student visible exams:', examIds);
    if (!examIds.includes('CS101') || !examIds.includes('CS202')) {
      throw new Error('Student did not see published exams');
    }
    console.log('   ✓ PASS: Students can access all published exams regardless of which admin created them.');

    console.log('\n====================================================');
    console.log('ALL 12 END-TO-END TESTS PASSED WITH 100% SUCCESS!');
    console.log('====================================================');

  } finally {
    serverProc.kill();
  }
}

run().catch(err => {
  console.error('\n❌ TEST FAILED:', err.message);
  process.exit(1);
});

