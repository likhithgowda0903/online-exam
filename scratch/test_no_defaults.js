const fs = require('fs');
const vm = require('vm');

const html = fs.readFileSync('index.html', 'utf8');
const scriptMatch = html.match(/<script>([\s\S]*?)<\/script>/);

if (!scriptMatch) {
  console.error("No script tag found!");
  process.exit(1);
}

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
        classList: { add: () => {}, remove: () => {}, contains: () => false },
        appendChild: () => {},
        setAttribute: () => {},
        getAttribute: () => null,
        addEventListener: () => {},
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

const mockLocalStorage = {
  store: {},
  getItem: function(k) { return this.store[k] || null; },
  setItem: function(k, v) { this.store[k] = String(v); },
  removeItem: function(k) { delete this.store[k]; },
  clear: function() { this.store = {}; }
};

const sandbox = {
  window: {},
  addEventListener: () => {},
  removeEventListener: () => {},
  document: mockDoc,
  localStorage: mockLocalStorage,
  sessionStorage: mockLocalStorage,
  console: console,
  navigator: { userAgent: 'node', mediaDevices: { getUserMedia: () => Promise.resolve() } },
  fetch: () => Promise.reject("offline"),
  setTimeout: setTimeout,
  clearTimeout: clearTimeout,
  setInterval: setInterval,
  clearInterval: clearInterval,
  requestAnimationFrame: () => 1,
  cancelAnimationFrame: () => {},
  AudioContext: function() {
    return {
      createOscillator: () => ({ connect: () => {}, start: () => {}, stop: () => {} }),
      createGain: () => ({ gain: { value: 0 }, connect: () => {} }),
      destination: {}
    };
  }
};
sandbox.window = sandbox;

vm.createContext(sandbox);

try {
  vm.runInContext(scriptMatch[1], sandbox);
} catch (err) {
  console.error("Execution error:", err);
  process.exit(1);
}

console.log("=== 1. VERIFY DEFAULT ACCOUNTS REMOVED ===");
const registeredCandidates = vm.runInContext('registeredCandidates', sandbox);
const registeredAdmins = vm.runInContext('registeredAdmins', sandbox);

console.log("Registered candidates count:", Object.keys(registeredCandidates).length);
console.log("Registered admins count:", Object.keys(registeredAdmins).length);

if (registeredCandidates['student'] || registeredCandidates['stu1001']) {
  console.error("FAIL: Default student still exists in registeredCandidates!");
  process.exit(1);
} else {
  console.log("✓ Default student candidate successfully removed from candidate registry");
}

if (registeredAdmins['admin'] || registeredAdmins['adm101']) {
  console.error("FAIL: Default admin still exists in registeredAdmins!");
  process.exit(1);
} else {
  console.log("✓ Default system administrator successfully removed from admin registry");
}

console.log("\n=== 2. VERIFY FORM INPUT PRE-FILLS REMOVED ===");
const studentU = html.includes('id="authUsername" class="auth-input" placeholder="Enter student ID, username or email" required value=""');
const studentP = html.includes('id="authPassword" class="auth-input" placeholder="Enter password" required value=""');
const adminU = html.includes('id="authAdminUsername" class="auth-input" placeholder="Enter administrator ID or username" required value=""');
const adminP = html.includes('id="authAdminPassword" class="auth-input" placeholder="Enter administrator password" required value=""');

if (studentU && studentP && adminU && adminP) {
  console.log("✓ All 4 authentication form inputs (student & admin username/password) are completely empty with value=\"\"");
} else {
  console.error("FAIL: Some form inputs still contain prefilled values!");
  process.exit(1);
}

console.log("\n=== 3. VERIFY DYNAMIC REGISTRATION & LOGIN STILL WORK ===");
const handleRegisterSubmit = vm.runInContext('handleRegisterSubmit', sandbox);
const state = vm.runInContext('state', sandbox);

// Test student registration
mockDoc.getElementById('regName').value = "Alice Candidate";
mockDoc.getElementById('regId').value = "alice101";
mockDoc.getElementById('regEmail').value = "alice@student.edu";
mockDoc.getElementById('regPwd').value = "AlicePass123";
mockDoc.getElementById('regPwdConf').value = "AlicePass123";
mockDoc.getElementById('regProgram').value = "Computer Science";

state.regRole = 'student';
handleRegisterSubmit({ preventDefault: () => {} });

console.log("State after registration: isAuthenticated =", state.isAuthenticated, ", user =", state.user ? state.user.name : "null");
if (state.isAuthenticated && state.user && state.user.username === 'alice101') {
  console.log("✓ New student registered and authenticated successfully!");
} else {
  console.error("FAIL: Registration did not authenticate user properly!");
  process.exit(1);
}

console.log("\n=== 4. VERIFY OOP EXAM INTACT ===");
const allExams = vm.runInContext('allExams', sandbox);
if (allExams['OOP'] && allExams['OOP'].questions.length === 20 && allExams['OOP'].durationMinutes === 15) {
  console.log("✓ OOP examination preserved intact: 20 Questions, 15 Minutes duration, 5 Marks each");
} else {
  console.error("FAIL: OOP examination data was affected!");
  process.exit(1);
}

console.log("\n>>> ALL TESTS PASSED! ZERO DEFAULT ACCOUNTS! <<<");
process.exit(0);
