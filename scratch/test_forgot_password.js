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

vm.createContext(sandbox);
vm.runInContext(scriptMatch[1], sandbox);

const state = vm.runInContext('state', sandbox);
const openStudentForgotPassword = vm.runInContext('openStudentForgotPassword', sandbox);
const handleVerifyStudent = vm.runInContext('handleVerifyStudent', sandbox);
const handleResetPassword = vm.runInContext('handleResetPassword', sandbox);
const handleRegisterSubmit = vm.runInContext('handleRegisterSubmit', sandbox);
const handleLoginSubmit = vm.runInContext('handleLoginSubmit', sandbox);
const handleSignOut = vm.runInContext('handleSignOut', sandbox);
const switchAuthTab = vm.runInContext('switchAuthTab', sandbox);

(async () => {
  console.log("=== 1. VERIFY FORGOT PASSWORD BUTTON & RECOVERY VIEW OPENING ===");
  if (html.includes('onclick="openStudentForgotPassword(event)"')) {
    console.log("✓ Forgot Password link correctly hooked to openStudentForgotPassword(event)");
  } else {
    console.error("FAIL: Forgot Password link not found or missing handler!");
    process.exit(1);
  }

  openStudentForgotPassword({ preventDefault: () => {} });
  if (state.authTab === 'forgot-password' && state.forgotStep === 1) {
    console.log("✓ openStudentForgotPassword transitioned state to authTab='forgot-password' & forgotStep=1");
  } else {
    console.error("FAIL: State not updated correctly on opening recovery view:", state);
    process.exit(1);
  }

  const viewport = mockDoc.getElementById('appViewport');
  if (viewport.innerHTML.includes('Forgot Student Password?') &&
      viewport.innerHTML.includes('recoverStudentRoll') &&
      viewport.innerHTML.includes('recoverStudentEmail') &&
      viewport.innerHTML.includes('Verify Account')) {
    console.log("✓ Recovery interface rendered with heading, Roll Number field, Email field, and Verify button");
  } else {
    console.error("FAIL: Recovery interface HTML missing elements!");
    process.exit(1);
  }

  switchAuthTab('signin');
  if (state.authTab === 'signin') {
    console.log("✓ Navigation back to Student Sign In functions properly");
  } else {
    console.error("FAIL: Back to Sign In did not reset authTab to 'signin'");
    process.exit(1);
  }

  console.log("\n=== 2. REGISTER A TEST STUDENT ===");
  mockDoc.getElementById('regName').value = "Maya Lin";
  mockDoc.getElementById('regId').value = "maya_lin";
  mockDoc.getElementById('regEmail').value = "maya@campus.edu";
  mockDoc.getElementById('regPwd').value = "MayaPass123";
  mockDoc.getElementById('regPwdConf').value = "MayaPass123";
  mockDoc.getElementById('regProgram').value = "Computer Science";
  state.regRole = 'student';
  handleRegisterSubmit({ preventDefault: () => {} });

  if (!state.isAuthenticated || !state.user) {
    console.error("FAIL: Could not register test student!");
    process.exit(1);
  }
  console.log("✓ Test student candidate registered successfully:", state.user.name, state.user.email);

  handleSignOut();

  console.log("\n=== 3. TEST ACCOUNT VERIFICATION ===");
  openStudentForgotPassword({ preventDefault: () => {} });

  // A. Test invalid roll number
  mockDoc.getElementById('recoverStudentRoll').value = "unknown_user_99";
  mockDoc.getElementById('recoverStudentEmail').value = "maya@campus.edu";
  await handleVerifyStudent({ preventDefault: () => {} });

  if (state.forgotStep !== 1 || state.verifiedCandidateKey !== null) {
    console.error("FAIL: Invalid roll number was incorrectly verified!");
    process.exit(1);
  }
  console.log("✓ Invalid student roll number correctly rejected");

  // B. Test invalid email
  mockDoc.getElementById('recoverStudentRoll').value = "maya_lin";
  mockDoc.getElementById('recoverStudentEmail').value = "wrong_email@campus.edu";
  await handleVerifyStudent({ preventDefault: () => {} });

  if (state.forgotStep !== 1 || state.verifiedCandidateKey !== null) {
    console.error("FAIL: Invalid email was incorrectly verified!");
    process.exit(1);
  }
  console.log("✓ Mismatched email address correctly rejected");

  // C. Test valid verification
  mockDoc.getElementById('recoverStudentRoll').value = "maya_lin";
  mockDoc.getElementById('recoverStudentEmail').value = "maya@campus.edu";
  await handleVerifyStudent({ preventDefault: () => {} });

  if (state.forgotStep === 2 && state.verifiedCandidateKey !== null) {
    console.log("✓ Valid student credentials verified successfully! Transitioned to Step 2 (Reset Password)");
  } else {
    console.error("FAIL: Valid student credentials failed verification!", state);
    process.exit(1);
  }

  console.log("\n=== 4. TEST PASSWORD RESET VALIDATION & COMPLETION ===");
  // A. Test short password (< 6 chars)
  mockDoc.getElementById('newStudentPassword').value = "123";
  mockDoc.getElementById('confirmStudentPassword').value = "123";
  await handleResetPassword({ preventDefault: () => {} });
  if (state.authTab !== 'forgot-password') {
    console.error("FAIL: Short password was accepted!");
    process.exit(1);
  }
  console.log("✓ Password under 6 characters correctly rejected");

  // B. Test mismatched passwords
  mockDoc.getElementById('newStudentPassword').value = "NewPass2026!";
  mockDoc.getElementById('confirmStudentPassword').value = "DifferentPass2026!";
  await handleResetPassword({ preventDefault: () => {} });
  if (state.authTab !== 'forgot-password') {
    console.error("FAIL: Mismatched passwords were accepted!");
    process.exit(1);
  }
  console.log("✓ Mismatched passwords correctly rejected");

  // C. Test valid password reset
  mockDoc.getElementById('newStudentPassword').value = "NewPass2026!";
  mockDoc.getElementById('confirmStudentPassword').value = "NewPass2026!";
  await handleResetPassword({ preventDefault: () => {} });

  if (state.authTab === 'signin' && state.forgotStep === 1 && state.verifiedCandidateKey === null) {
    console.log("✓ Password successfully reset and user redirected to Sign In tab");
  } else {
    console.error("FAIL: Password reset did not redirect to signin cleanly:", state);
    process.exit(1);
  }

  console.log("\n=== 5. VERIFY OLD PASSWORD REJECTED & NEW PASSWORD LOGS IN ===");
  // A. Try logging in with old password
  mockDoc.getElementById('authUsername').value = "maya_lin";
  mockDoc.getElementById('authPassword').value = "MayaPass123";
  handleLoginSubmit({ preventDefault: () => {} });

  if (state.isAuthenticated) {
    console.error("FAIL: Old password was accepted!");
    process.exit(1);
  }
  console.log("✓ Old password (MayaPass123) correctly rejected");

  // B. Try logging in with new password
  mockDoc.getElementById('authPassword').value = "NewPass2026!";
  handleLoginSubmit({ preventDefault: () => {} });

  if (state.isAuthenticated && state.user && state.user.username === 'maya_lin') {
    console.log("✓ New password (NewPass2026!) authenticated successfully into Student Dashboard!");
  } else {
    console.error("FAIL: New password did not log in!");
    process.exit(1);
  }

  console.log("\n>>> ALL 7 FORGOT PASSWORD ACCEPTANCE TESTS PASSED! 100% VERIFIED! <<<");
  process.exit(0);
})();
