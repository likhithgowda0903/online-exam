const { spawn } = require('child_process');

const payload = {
  id: "TEST101",
  title: "Test Exam Title",
  category: "Computer Science",
  totalQuestions: 0,
  durationMinutes: 45,
  passingPercentage: 60,
  totalMarks: 100,
  created_by: "ADM101",
  admin_id: "ADM101",
  questions: []
};

const proc = spawn('python', ['sqlite_bridge.py', 'save_exam', 'ADM101']);
let out = '';
let err = '';
proc.stdout.on('data', d => out += d.toString());
proc.stderr.on('data', d => err += d.toString());
proc.on('close', code => {
  console.log('Exit code:', code);
  console.log('Output:', out);
  console.log('Error:', err);

  // Now check exams in DB
  const proc2 = spawn('python', ['sqlite_bridge.py', 'exams', 'ADM101']);
  let out2 = '';
  proc2.stdout.on('data', d => out2 += d.toString());
  proc2.on('close', () => {
    console.log('Exams for ADM101:', out2);
  });
});

proc.stdin.write(JSON.stringify(payload));
proc.stdin.end();

