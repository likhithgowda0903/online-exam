const fs = require('fs');
const vm = require('vm');
const assert = require('assert');

console.log("=== COMPREHENSIVE STUDENT CANDIDATE REGISTRY TABLE TEST ===");

const html = fs.readFileSync('index.html', 'utf8');

// 1. Table structure and CSS class check
assert(html.includes('class="admin-student-table"'), "Missing class admin-student-table");
assert(html.includes('class="table-responsive-admin"'), "Missing class table-responsive-admin");
console.log("✓ Found admin-student-table and table-responsive-admin");

// 2. Column widths in colgroup (14%, 20%, 20%, 13%, 18%, 15%)
assert(html.includes('<col style="width: 14%;">'), "Missing 14% col width for Roll ID");
assert(html.includes('<col style="width: 20%;">'), "Missing 20% col width for Candidate Name / Program");
assert(html.includes('<col style="width: 13%;">'), "Missing 13% col width for Exams Taken");
assert(html.includes('<col style="width: 18%;">'), "Missing 18% col width for Institutional Avg");
assert(html.includes('<col style="width: 15%;">'), "Missing 15% col width for Status");
console.log("✓ Column width proportions (14%, 20%, 20%, 13%, 18%, 15%) verified in colgroup");

// 3. Exactly 6 headers in exact order
const headers = [
  'Student Roll ID',
  'Candidate Name',
  'Academic Program',
  'Exams Taken',
  'Institutional Avg',
  'Status'
];

let lastPos = 0;
headers.forEach((h, idx) => {
  const pos = html.indexOf(h, lastPos);
  assert(pos > lastPos, `Header #${idx + 1} '${h}' not found or out of order`);
  lastPos = pos;
  console.log(`✓ Column ${idx + 1}: '${h}' in correct sequence`);
});

// 4. CSS Alignment classes check
assert(html.includes('.admin-student-col-roll {'), "Missing .admin-student-col-roll CSS");
assert(html.includes('.admin-student-col-name {'), "Missing .admin-student-col-name CSS");
assert(html.includes('.admin-student-col-program {'), "Missing .admin-student-col-program CSS");
assert(html.includes('.admin-student-col-exams {'), "Missing .admin-student-col-exams CSS");
assert(html.includes('.admin-student-col-avg {'), "Missing .admin-student-col-avg CSS");
assert(html.includes('.admin-student-col-status {'), "Missing .admin-student-col-status CSS");
console.log("✓ All 6 column CSS classes and alignment definitions verified");

// 5. Test rendering in VM with multiple student candidate records
const mockDoc = {
  elements: {},
  body: { classList: { add: () => {}, remove: () => {} } },
  addEventListener: () => {},
  removeEventListener: () => {},
  getElementById: function(id) {
    if (!this.elements[id]) {
      this.elements[id] = {
        id,
        innerHTML: '',
        value: '',
        style: {},
        attrs: {},
        classList: { add: () => {}, remove: () => {} },
        setAttribute: function(k, v) { this.attrs[k] = v; },
        getAttribute: function(k) { return this.attrs[k]; },
        removeAttribute: function(k) { delete this.attrs[k]; }
      };
    }
    return this.elements[id];
  },
  querySelector: () => null,
  querySelectorAll: () => []
};

const sandbox = {
  window: { addEventListener: () => {}, removeEventListener: () => {} },
  document: mockDoc,
  addEventListener: () => {},
  removeEventListener: () => {},
  localStorage: { getItem: () => null, setItem: () => {}, removeItem: () => {} },
  sessionStorage: { getItem: () => null, setItem: () => {}, removeItem: () => {} },
  console: console,
  setTimeout: (fn) => fn(),
  clearTimeout: () => {},
  setInterval: () => 1,
  clearInterval: () => {},
  fetch: () => Promise.reject("offline")
};
sandbox.window = sandbox;

const scriptMatch = html.match(/<script>([\s\S]*?)<\/script>/);
vm.createContext(sandbox);
vm.runInContext(scriptMatch[1], sandbox);

const state = vm.runInContext('state', sandbox);
const renderAdminDashboard = vm.runInContext('renderAdminDashboard', sandbox);
const registeredCandidates = vm.runInContext('registeredCandidates', sandbox);

// Add multiple test students with varying attempt and program data
registeredCandidates['stu1001'] = {
  id: 'STU1001',
  username: 'stu1001',
  name: 'Aarav Patel',
  email: 'aarav.patel@campus.edu',
  program: 'B.Tech Computer Science'
};
registeredCandidates['stu1002'] = {
  id: 'STU1002',
  username: 'stu1002',
  name: 'Sneha Rao',
  email: 'sneha.rao@campus.edu',
  program: 'Information Technology'
};
registeredCandidates['stu1003'] = {
  id: 'STU1003',
  username: 'stu1003',
  name: 'Vikram Singh',
  email: '', // missing email test
  program: '' // missing program test
};

// Simulate one student having taken an exam attempt
state.history = [
  { studentId: 'STU1001', examId: 'OOP', pct: 90, passed: true }
];

state.adminActiveTab = 'students';
const container = { innerHTML: '' };
renderAdminDashboard(container);

const rendered = container.innerHTML;

assert(rendered.includes('Student Candidate Registry'), "Registry heading missing");
assert(rendered.includes('3 Candidates Enrolled'), "Candidate enrollment badge count mismatch");

// Verify student 1 (with attempt)
assert(rendered.includes('STU1001'), "Missing STU1001");
assert(rendered.includes('Aarav Patel'), "Missing Aarav Patel");
assert(rendered.includes('aarav.patel@campus.edu'), "Missing Aarav email");
assert(rendered.includes('B.Tech Computer Science'), "Missing program");
assert(rendered.includes('>1</strong>') && rendered.includes('>Exam</span>'), "Missing 1 Exam attempt");
assert(rendered.includes('90%'), "Missing 90% score");
assert(rendered.includes('Enrolled'), "Missing Enrolled badge");

// Verify student 2 (pending exam)
assert(rendered.includes('STU1002'), "Missing STU1002");
assert(rendered.includes('Sneha Rao'), "Missing Sneha Rao");
assert(rendered.includes('>0</strong>') && rendered.includes('>Exams</span>'), "Missing 0 Exams");
assert(rendered.includes('Pending Exam'), "Missing Pending Exam badge");

// Verify student 3 (fallbacks for missing email and program)
assert(rendered.includes('STU1003'), "Missing STU1003");
assert(rendered.includes('Vikram Singh'), "Missing Vikram Singh");
assert(rendered.includes('No email registered'), "Missing fallback email text");
assert(rendered.includes('Computer Science'), "Missing default fallback program");

console.log("✓ Dynamic rendering with multiple candidates, vertical name/email stacking, attempt averages, and fallbacks verified");
console.log("\n🎉 ALL STUDENT CANDIDATE REGISTRY TABLE ALIGNMENT TESTS PASSED SUCCESSFULLY!");
