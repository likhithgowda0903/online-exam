const fs = require('fs');
const vm = require('vm');
const assert = require('assert');

console.log("=== COMPREHENSIVE EXAMINATION TABLE ALIGNMENT TEST ===");

const html = fs.readFileSync('index.html', 'utf8');

// 1. Check Table Structure & Class
assert(html.includes('class="admin-exam-table"'), "Missing class admin-exam-table");
assert(html.includes('class="table-responsive-admin"'), "Missing class table-responsive-admin");
console.log("✓ Found admin-exam-table and table-responsive-admin classes");

// 2. Check Column Group Proportions
const colgroupMatch = html.match(/<colgroup>([\s\S]*?)<\/colgroup>/);
assert(colgroupMatch, "Missing colgroup element");
const colgroup = colgroupMatch[1];
assert(colgroup.includes('style="width: 25%;"'), "Missing 25% col width for Title");
assert(colgroup.includes('style="width: 13%;"'), "Missing 13% col width for Category");
assert(colgroup.includes('style="width: 12%;"'), "Missing 12% col width for Questions");
assert(colgroup.includes('style="width: 8%;"'), "Missing 8% col width for Duration");
assert(colgroup.includes('style="width: 7%;"'), "Missing 7% col width for Pass %");
assert(colgroup.includes('style="width: 9%;"'), "Missing 9% col width for Total Marks");
assert(colgroup.includes('style="width: 26%;"'), "Missing 26% col width for Actions");
console.log("✓ Recommended column proportions (25%, 13%, 12%, 8%, 7%, 9%, 26%) exactly matched");

// 3. Check All 7 Column Headers in Order
const headers = [
  'Exam Code & Title',
  'Category',
  'Questions in Pool',
  'Duration',
  'Pass %',
  'Total Marks',
  'Administrative Actions'
];

let lastPos = 0;
headers.forEach((h, idx) => {
  const pos = html.indexOf(h, lastPos);
  assert(pos > lastPos, `Header #${idx + 1} '${h}' not found or out of order`);
  lastPos = pos;
  console.log(`✓ Column ${idx + 1}: '${h}' in correct sequence`);
});

// 4. Check CSS definitions
assert(html.includes('.table-responsive-admin {'), "Missing .table-responsive-admin CSS");
assert(html.includes('.admin-exam-table {'), "Missing .admin-exam-table CSS");
assert(html.includes('.admin-action-btn-group {'), "Missing .admin-action-btn-group CSS");
assert(html.includes('.admin-exam-btn {'), "Missing .admin-exam-btn CSS");
assert(html.includes('table-layout: fixed;'), "Missing table-layout: fixed");
assert(html.includes('min-width: 960px;'), "Missing min-width: 960px");
console.log("✓ CSS rules for responsive layout, fixed proportions, and button sizing verified");

// 5. Check Action Buttons
assert(html.includes('Manage Questions'), "Missing Manage Questions button");
assert(html.includes('Preview'), "Missing Preview button");
assert(html.includes('Delete Exam'), "Missing Delete Exam button");
assert(html.includes("onclick=\"switchAdminExamPool('${e.id}'); switchAdminTab('questions');\""), "Manage Questions handler modified");
assert(html.includes("onclick=\"state.selectedExamId = '${e.id}'; showView('exam-room');\""), "Preview handler modified");
assert(html.includes("onclick=\"deleteExam('${e.id}')\""), "Delete Exam handler modified");
console.log("✓ Action buttons and existing onclick handlers preserved");

// 6. Test Rendering in VM
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
const allExams = vm.runInContext('allExams', sandbox);

state.adminActiveTab = 'quizzes';
const container = { innerHTML: '' };
renderAdminDashboard(container);

assert(container.innerHTML.includes('class="admin-exam-table"'), "Rendered view missing admin-exam-table");
assert(container.innerHTML.includes('Official Course Quizzes & Modules'), "Rendered view missing section title");

if (Object.keys(allExams).length > 0) {
  const firstExam = Object.values(allExams)[0];
  assert(container.innerHTML.includes(firstExam.title), `Rendered table missing exam title '${firstExam.title}'`);
  assert(container.innerHTML.includes('Manage Questions'), "Rendered row missing Manage Questions button");
  assert(container.innerHTML.includes('Preview'), "Rendered row missing Preview button");
  assert(container.innerHTML.includes('Delete Exam'), "Rendered row missing Delete Exam button");
  console.log(`✓ Real examination '${firstExam.title}' (${firstExam.id}) successfully rendered with all columns and actions`);
}

console.log("\n🎉 ALL EXAMINATION TABLE ALIGNMENT VERIFICATIONS PASSED SUCCESSFULLY!");
