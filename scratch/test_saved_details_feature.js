const fs = require('fs');
const vm = require('vm');
const assert = require('assert');

console.log("=== COMPREHENSIVE SAVED DETAILS ON-CLICK TEST ===");

const html = fs.readFileSync('index.html', 'utf8');

// Verify DOM structure
assert(html.includes('id="btnStudentSavedDetails"'), "Missing #btnStudentSavedDetails button");
assert(html.includes('id="btnAdminSavedDetails"'), "Missing #btnAdminSavedDetails button");
assert(html.includes('id="savedDetailsDropdown_student"'), "Missing #savedDetailsDropdown_student");
assert(html.includes('id="savedDetailsDropdown_admin"'), "Missing #savedDetailsDropdown_admin");
assert(html.includes('onclick="handleAuthInputClick(this, \'student\', event)"'), "Missing handleAuthInputClick on student username");
assert(html.includes('onclick="handleAuthInputClick(this, \'admin\', event)"'), "Missing handleAuthInputClick on admin username");

console.log("✓ All required DOM elements and on-click handlers verified in HTML");

const mockDoc = {
  elements: {},
  body: {
    appendChild: () => {},
    removeChild: () => {},
    classList: { add: () => {}, remove: () => {} }
  },
  addEventListener: () => {},
  removeEventListener: () => {},
  getElementById: function(id) {
    if (!this.elements[id]) {
      this.elements[id] = {
        id,
        innerHTML: '',
        textContent: '',
        value: '',
        style: {},
        attrs: {},
        classList: { add: () => {}, remove: () => {}, contains: () => false },
        setAttribute: function(name, val) { this.attrs[name] = val; },
        getAttribute: function(name) { return this.attrs[name] || null; },
        removeAttribute: function(name) { delete this.attrs[name]; },
        appendChild: () => {},
        addEventListener: () => {},
        contains: () => false,
        focus: () => {}
      };
    }
    return this.elements[id];
  },
  querySelector: () => null,
  querySelectorAll: () => [],
  createElement: function(tag) {
    return {
      tagName: tag,
      style: {},
      classList: { add: () => {}, remove: () => {} },
      appendChild: () => {},
      remove: () => {}
    };
  }
};

let toastMessages = [];

const sandbox = {
  window: {},
  addEventListener: () => {},
  removeEventListener: () => {},
  document: mockDoc,
  localStorage: {
    store: {},
    getItem: function(k) { return this.store[k] || null; },
    setItem: function(k, v) { this.store[k] = String(v); },
    removeItem: function(k) { delete this.store[k]; }
  },
  sessionStorage: { getItem: () => null, setItem: () => {}, removeItem: () => {} },
  console: console,
  navigator: { userAgent: 'node', mediaDevices: { getUserMedia: () => Promise.resolve() } },
  fetch: () => Promise.reject("offline"),
  setTimeout: (fn) => fn(),
  clearTimeout: () => {},
  setInterval: () => 1,
  clearInterval: () => {},
  requestAnimationFrame: () => 1,
  cancelAnimationFrame: () => {}
};
sandbox.window = sandbox;

const scriptMatch = html.match(/<script>([\s\S]*?)<\/script>/);
vm.createContext(sandbox);
vm.runInContext(scriptMatch[1], sandbox);

sandbox.showToastNotification = (msg) => { toastMessages.push(msg); };

const state = vm.runInContext('state', sandbox);
const registeredAdmins = vm.runInContext('registeredAdmins', sandbox);
const registeredCandidates = vm.runInContext('registeredCandidates', sandbox);
const unlockAuthInput = vm.runInContext('unlockAuthInput', sandbox);
const handleAuthInputClick = vm.runInContext('handleAuthInputClick', sandbox);
const toggleSavedDetailsDropdown = vm.runInContext('toggleSavedDetailsDropdown', sandbox);
const closeAllSavedDetailsDropdowns = vm.runInContext('closeAllSavedDetailsDropdowns', sandbox);
const showSavedDetailsDropdown = vm.runInContext('showSavedDetailsDropdown', sandbox);
const selectSavedAccount = vm.runInContext('selectSavedAccount', sandbox);
const enforceCleanAuthFieldsOnEntry = vm.runInContext('enforceCleanAuthFieldsOnEntry', sandbox);
const showView = vm.runInContext('showView', sandbox);

(() => {
  // Test 1: On entering website, fields are empty and readonly
  console.log("\n[Test 1] Testing Clean Fields on Website Entry...");
  // Simulate having some junk values in DOM
  mockDoc.getElementById('authUsername').value = 'prefilled_user';
  mockDoc.getElementById('authPassword').value = 'secret123';
  mockDoc.getElementById('authAdminUsername').value = 'prefilled_admin';
  mockDoc.getElementById('authAdminPassword').value = 'adminpass';

  // Enter website
  showView('auth');

  assert.strictEqual(mockDoc.getElementById('authUsername').value, '', "authUsername must be cleared");
  assert.strictEqual(mockDoc.getElementById('authPassword').value, '', "authPassword must be cleared");
  assert.strictEqual(mockDoc.getElementById('authAdminUsername').value, '', "authAdminUsername must be cleared");
  assert.strictEqual(mockDoc.getElementById('authAdminPassword').value, '', "authAdminPassword must be cleared");

  assert.strictEqual(mockDoc.getElementById('authUsername').getAttribute('readonly'), 'readonly', "authUsername must be readonly on entry");
  assert.strictEqual(mockDoc.getElementById('authPassword').getAttribute('readonly'), 'readonly', "authPassword must be readonly on entry");
  console.log("✓ On entering website: All username and password fields are clean and protected against auto-display");

  // Test 2: Clicking input field removes readonly and opens saved details
  console.log("\n[Test 2] Testing Clicking Input Field...");
  const uField = mockDoc.getElementById('authUsername');
  handleAuthInputClick(uField, 'student', { stopPropagation: () => {} });
  assert.strictEqual(uField.getAttribute('readonly'), null, "readonly must be removed on click");
  const studentDrop = mockDoc.getElementById('savedDetailsDropdown_student');
  assert.strictEqual(studentDrop.style.display, 'block', "Student saved details dropdown must open on click");
  console.log("✓ Clicking input field unlocked the field for typing and opened Saved Details dropdown");

  // Test 3: Saved Details contains registered & remembered accounts
  console.log("\n[Test 3] Testing Saved Details Population...");
  // Register an account
  registeredCandidates['stu2026'] = {
    id: 'STU2026',
    username: 'stu2026',
    name: 'Rahul Sharma',
    email: 'rahul@campus.edu',
    password: 'RahulSecurePass!'
  };

  showSavedDetailsDropdown('student');
  assert(studentDrop.innerHTML.includes('Rahul Sharma'), "Dropdown must display Rahul Sharma");
  assert(studentDrop.innerHTML.includes('STU2026'), "Dropdown must display STU2026");
  console.log("✓ Saved Details dropdown correctly lists registered account details");

  // Test 4: Selecting a saved account fills username & password
  console.log("\n[Test 4] Testing Account Selection...");
  toastMessages = [];
  selectSavedAccount('STU2026', 'RahulSecurePass!', 'student', 'Rahul Sharma');
  assert.strictEqual(mockDoc.getElementById('authUsername').value, 'STU2026', "Username must be filled with selected account ID");
  assert.strictEqual(mockDoc.getElementById('authPassword').value, 'RahulSecurePass!', "Password must be filled with selected account password");
  assert.strictEqual(studentDrop.style.display, 'none', "Dropdown must close after selection");
  assert(toastMessages.some(m => m.includes("Filled saved details for Rahul Sharma")), "Toast feedback must be displayed");
  console.log("✓ Clicking saved account cleanly populates credentials and closes dropdown");

  // Test 5: Administrator Saved Details
  console.log("\n[Test 5] Testing Administrator Saved Details...");
  registeredAdmins['hms101'] = {
    id: 'HMS101',
    username: 'hms101',
    name: 'Hamsa Faculty',
    email: 'hamsa@faculty.edu',
    password: 'HamsaAdminPassword!'
  };
  const adminField = mockDoc.getElementById('authAdminUsername');
  handleAuthInputClick(adminField, 'admin', { stopPropagation: () => {} });
  const adminDrop = mockDoc.getElementById('savedDetailsDropdown_admin');
  assert.strictEqual(adminDrop.style.display, 'block', "Admin saved details dropdown must open");
  assert(adminDrop.innerHTML.includes('Hamsa Faculty'), "Admin dropdown must display Hamsa Faculty");

  selectSavedAccount('HMS101', 'HamsaAdminPassword!', 'admin', 'Hamsa Faculty');
  assert.strictEqual(mockDoc.getElementById('authAdminUsername').value, 'HMS101', "Admin username must be filled");
  assert.strictEqual(mockDoc.getElementById('authAdminPassword').value, 'HamsaAdminPassword!', "Admin password must be filled");
  assert.strictEqual(adminDrop.style.display, 'none', "Admin dropdown must close");
  console.log("✓ Administrator saved details on-click works identically and cleanly");

  console.log("\n🎉 ALL SAVED DETAILS ON-CLICK VERIFICATIONS PASSED SUCCESSFULLY!");
})();

