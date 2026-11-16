const fs = require('fs');
const vm = require('vm');
const assert = require('assert');

console.log("=== COMPREHENSIVE ADMINISTRATOR FORGOT PASSWORD TEST ===");

const html = fs.readFileSync('index.html', 'utf8');

// Verify DOM structure in html
assert(html.includes('id="authAdminForgotPanel"'), "Missing #authAdminForgotPanel");
assert(html.includes('id="adminForgotStepVerify"'), "Missing #adminForgotStepVerify");
assert(html.includes('id="adminForgotStepReset"'), "Missing #adminForgotStepReset");
assert(html.includes('id="recoverAdminStaffId"'), "Missing #recoverAdminStaffId");
assert(html.includes('id="recoverAdminEmail"'), "Missing #recoverAdminEmail");
assert(html.includes('id="newAdminPassword"'), "Missing #newAdminPassword");
assert(html.includes('id="confirmAdminPassword"'), "Missing #confirmAdminPassword");
assert(html.includes('openAdminForgotPassword(event)'), "Missing openAdminForgotPassword event call");
assert(html.includes('handleVerifyAdmin(event)'), "Missing handleVerifyAdmin event call");
assert(html.includes('handleResetAdminPassword(event)'), "Missing handleResetAdminPassword event call");

console.log("✓ All required DOM elements and event listeners present in index.html");

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

const scriptMatch = html.match(/<script>([\s\S]*?)<\/script>/);
vm.createContext(sandbox);
vm.runInContext(scriptMatch[1], sandbox);

// Override showToastNotification to capture messages
sandbox.showToastNotification = (msg) => { toastMessages.push(msg); };

const state = vm.runInContext('state', sandbox);
const registeredAdmins = vm.runInContext('registeredAdmins', sandbox);
const registeredCandidates = vm.runInContext('registeredCandidates', sandbox);
const openAdminForgotPassword = vm.runInContext('openAdminForgotPassword', sandbox);
const handleVerifyAdmin = vm.runInContext('handleVerifyAdmin', sandbox);
const handleResetAdminPassword = vm.runInContext('handleResetAdminPassword', sandbox);
const switchAuthTab = vm.runInContext('switchAuthTab', sandbox);
const handleAdminLoginSubmit = vm.runInContext('handleAdminLoginSubmit', sandbox);
const handleLoginSubmit = vm.runInContext('handleLoginSubmit', sandbox);
const openStudentForgotPassword = vm.runInContext('openStudentForgotPassword', sandbox);

(async () => {
  // Test 1: openAdminForgotPassword
  console.log("\n[Test 1] Testing openAdminForgotPassword...");
  openAdminForgotPassword();
  assert.strictEqual(state.authTab, 'forgot-password-admin', "authTab should be forgot-password-admin");
  assert.strictEqual(state.adminForgotStep, 1, "adminForgotStep should be 1");
  console.log("✓ openAdminForgotPassword switches tab and initializes step 1");

  // Test 2: Back to sign in
  console.log("\n[Test 2] Testing Back to Sign In...");
  switchAuthTab('admin');
  assert.strictEqual(state.authTab, 'admin', "authTab should return to admin");
  assert.strictEqual(state.adminForgotStep, 1, "adminForgotStep reset to 1");
  console.log("✓ Back to Sign In cleanly resets step and returns to admin login");

  // Test 3: Account verification
  console.log("\n[Test 3] Testing Account Verification...");
  registeredAdmins['hamsa_admin'] = {
    id: 'HMS101',
    username: 'hamsa_admin',
    name: 'Hamsa Faculty Admin',
    role: 'admin',
    roleLabel: 'Exam Administrator',
    avatar: 'HA',
    email: 'hamsa@faculty.edu',
    dept: 'SOE',
    password: 'InitialPassword123'
  };

  mockDoc.getElementById('recoverAdminStaffId').value = 'HMS101';
  mockDoc.getElementById('recoverAdminEmail').value = 'wrong@faculty.edu';

  // Verify wrong email fails
  toastMessages = [];
  sandbox.fetch = async () => ({ ok: false, json: async () => ({ verified: false }) });
  await handleVerifyAdmin({ preventDefault: () => {} });
  assert(toastMessages.some(m => m.includes("Account verification failed")), "Should fail with wrong email");
  console.log("✓ Rejected verification with invalid administrator email");

  // Verify correct details succeeds
  mockDoc.getElementById('recoverAdminEmail').value = 'hamsa@faculty.edu';
  toastMessages = [];
  await handleVerifyAdmin({ preventDefault: () => {} });
  assert.strictEqual(state.adminForgotStep, 2, "adminForgotStep should advance to 2");
  assert.strictEqual(state.verifiedAdminKey, 'hamsa_admin', "verifiedAdminKey should match admin username");
  console.log("✓ Valid administrator verified successfully and transitioned to Step 2");

  // Test 4: Password Validation
  console.log("\n[Test 4] Testing Password Validation...");
  mockDoc.getElementById('newAdminPassword').value = '123';
  mockDoc.getElementById('confirmAdminPassword').value = '123';
  toastMessages = [];
  await handleResetAdminPassword({ preventDefault: () => {} });
  assert(toastMessages.some(m => m.includes("at least 6 characters")), "Should reject password < 6 chars");
  console.log("✓ Rejected short password (< 6 characters)");

  mockDoc.getElementById('newAdminPassword').value = 'SecurePass2026';
  mockDoc.getElementById('confirmAdminPassword').value = 'MismatchPass2026';
  toastMessages = [];
  await handleResetAdminPassword({ preventDefault: () => {} });
  assert(toastMessages.some(m => m.includes("Passwords do not match")), "Should reject mismatched passwords");
  console.log("✓ Rejected mismatched passwords");

  // Test 5: Successful Password Reset
  console.log("\n[Test 5] Testing Password Reset Success...");
  mockDoc.getElementById('newAdminPassword').value = 'HamsaSecureAdminPass99!';
  mockDoc.getElementById('confirmAdminPassword').value = 'HamsaSecureAdminPass99!';
  
  let resetPayload = null;
  sandbox.fetch = async (url, opts) => {
    if (url === '/api/reset-password') {
      resetPayload = JSON.parse(opts.body);
    }
    return { ok: true, json: async () => ({ success: true }) };
  };
  toastMessages = [];
  await handleResetAdminPassword({ preventDefault: () => {} });
  assert.strictEqual(registeredAdmins['hamsa_admin'].password, 'HamsaSecureAdminPass99!', "Password in registeredAdmins must be updated");
  assert(resetPayload !== null, "Reset password API should be called");
  assert.strictEqual(resetPayload.role, 'admin', "API call role should be admin");
  assert.strictEqual(resetPayload.identifier, 'hamsa_admin', "API call identifier should be hamsa_admin");
  assert.strictEqual(resetPayload.new_password, 'HamsaSecureAdminPass99!', "API call new_password should match");
  assert(toastMessages.some(m => m.includes("Administrator password reset successfully")), "Success toast should be shown");
  console.log("✓ Administrator password securely updated in registeredAdmins, storage, and SQLite API payload");

  // Test 6: Sign in with new password
  console.log("\n[Test 6] Testing Login With Updated Password...");
  mockDoc.getElementById('authAdminUsername').value = 'hamsa_admin';
  mockDoc.getElementById('authAdminPassword').value = 'InitialPassword123'; // Old password
  toastMessages = [];
  handleAdminLoginSubmit({ preventDefault: () => {} });
  assert(toastMessages.some(m => m.includes("Incorrect administrator password")), "Old password must fail");
  console.log("✓ Old password properly rejected on login");

  domValues = {
    authAdminPassword: 'HamsaSecureAdminPass99!' // New password
  };
  mockDoc.getElementById('authAdminPassword').value = 'HamsaSecureAdminPass99!';
  toastMessages = [];
  handleAdminLoginSubmit({ preventDefault: () => {} });
  assert.strictEqual(state.user.role, 'admin', "Current user should be logged in as admin");
  assert.strictEqual(state.user.username, 'hamsa_admin', "Logged in username should be hamsa_admin");
  console.log("✓ New password successfully authenticates into Administrator Dashboard");

  // Test 7: Student flow unaffected
  console.log("\n[Test 7] Verifying Student Flow Unaffected...");
  registeredCandidates['student1'] = {
    id: 'STU101',
    username: 'student1',
    name: 'Alice Student',
    role: 'student',
    roleLabel: 'Registered Candidate',
    avatar: 'AS',
    email: 'alice@examportal.edu',
    password: 'AlicePassword123'
  };
  openStudentForgotPassword();
  assert.strictEqual(state.authTab, 'forgot-password', "Student forgot password tab active");
  switchAuthTab('student');
  assert.strictEqual(state.authTab, 'student', "Student login tab active");
  mockDoc.getElementById('authUsername').value = 'student1';
  mockDoc.getElementById('authPassword').value = 'AlicePassword123';
  handleLoginSubmit({ preventDefault: () => {} });
  assert.strictEqual(state.user.role, 'student', "Student authenticates normally");
  console.log("✓ Student login and student forgot password flow remain 100% operational");

  console.log("\n🎉 ALL 7 ACCEPTANCE CRITERIA VERIFIED AND PASSED SUCCESSFULLY!");
})();
