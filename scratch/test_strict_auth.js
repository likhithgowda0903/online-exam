const fs = require('fs');
const vm = require('vm');

const html = fs.readFileSync('index.html', 'utf8');
const scriptMatch = html.match(/<script>([\s\S]*?)<\/script>/);

let lastToast = '';
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
vm.runInContext(scriptMatch[1], sandbox);

const handleLoginSubmit = vm.runInContext('handleLoginSubmit', sandbox);
const handleAdminLoginSubmit = vm.runInContext('handleAdminLoginSubmit', sandbox);
const handleRegisterSubmit = vm.runInContext('handleRegisterSubmit', sandbox);
const state = vm.runInContext('state', sandbox);

console.log("=== 1. TEST UNREGISTERED LOGIN REJECTION ===");
// Try logging in with "student" / "password"
mockDoc.getElementById('authUsername').value = "student";
mockDoc.getElementById('authPassword').value = "password";
handleLoginSubmit({ preventDefault: () => {} });

if (state.isAuthenticated) {
  console.error("FAIL: Default student login was accepted!");
  process.exit(1);
} else {
  console.log("✓ Default student / password correctly rejected (not authenticated)");
}

// Try logging in with "admin" / "admin"
mockDoc.getElementById('authAdminUsername').value = "admin";
mockDoc.getElementById('authAdminPassword').value = "admin";
handleAdminLoginSubmit({ preventDefault: () => {} });

if (state.isAuthenticated) {
  console.error("FAIL: Default admin login was accepted!");
  process.exit(1);
} else {
  console.log("✓ Default admin / admin correctly rejected (not authenticated)");
}

console.log("\n=== 2. TEST REGISTER THEN LOGIN ===");
// Register new student: John Doe (john_doe, MySecurePass)
mockDoc.getElementById('regName').value = "John Doe";
mockDoc.getElementById('regId').value = "john_doe";
mockDoc.getElementById('regEmail').value = "john@univ.edu";
mockDoc.getElementById('regPwd').value = "MySecurePass";
mockDoc.getElementById('regPwdConf').value = "MySecurePass";
mockDoc.getElementById('regProgram').value = "Computer Science";
state.regRole = 'student';
handleRegisterSubmit({ preventDefault: () => {} });

if (!state.isAuthenticated || state.user.username !== 'john_doe') {
  console.error("FAIL: Registration did not succeed!");
  process.exit(1);
}
console.log("✓ Real candidate registered successfully:", state.user.name);

// Sign out
const handleSignOut = vm.runInContext('handleSignOut', sandbox);
handleSignOut();
console.log("State after sign out: isAuthenticated =", state.isAuthenticated);

// Test wrong password
mockDoc.getElementById('authUsername').value = "john_doe";
mockDoc.getElementById('authPassword').value = "WrongPassword";
handleLoginSubmit({ preventDefault: () => {} });
if (state.isAuthenticated) {
  console.error("FAIL: Wrong password was accepted!");
  process.exit(1);
}
console.log("✓ Wrong password correctly rejected");

// Test correct password
mockDoc.getElementById('authPassword').value = "MySecurePass";
handleLoginSubmit({ preventDefault: () => {} });
if (!state.isAuthenticated || state.user.username !== 'john_doe') {
  console.error("FAIL: Correct password was not accepted!");
  process.exit(1);
}
console.log("✓ Correct registered credentials authenticated successfully!");

console.log("\n>>> STRICT AUTHENTICATION TESTS PASSED! <<<");
process.exit(0);

