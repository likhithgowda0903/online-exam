const fs = require('fs');
const vm = require('vm');

const html = fs.readFileSync('web-portal/index.html', 'utf8');

const scriptStart = html.indexOf('<script>');
const scriptEnd = html.lastIndexOf('</script>');
const scriptContent = html.substring(scriptStart + 8, scriptEnd);

function makeMockEl() {
  return {
    value: '',
    textContent: '',
    innerHTML: '',
    style: {},
    classList: { add: () => {}, remove: () => {} },
    focus: () => {},
    setAttribute: () => {},
    removeAttribute: () => {},
    getAttribute: () => null,
    addEventListener: () => {},
    remove: () => {},
    appendChild: () => {}
  };
}

const domMocks = {
  document: {
    getElementById: () => makeMockEl(),
    querySelector: () => makeMockEl(),
    querySelectorAll: () => [makeMockEl()],
    addEventListener: () => {},
    createElement: () => makeMockEl(),
    body: makeMockEl()
  },
  localStorage: {
    store: {},
    getItem: function(key) { return this.store[key] || null; },
    setItem: function(key, val) { this.store[key] = String(val); },
    removeItem: function(key) { delete this.store[key]; }
  },
  console: console,
  setTimeout: setTimeout,
  clearTimeout: clearTimeout,
  setInterval: setInterval,
  clearInterval: clearInterval,
  fetch: () => Promise.resolve({ ok: false })
};

const context = vm.createContext(domMocks);
try {
  vm.runInContext(scriptContent, context);
  console.log('Script loaded successfully in VM!');

  // Test inside VM
  const testScript = `
    console.log("Initial state authenticated:", state.isAuthenticated);

    // Scenario 1: New Admin A registers
    const adminA = {
      id: "ADM101",
      username: "adm101",
      role: "admin",
      roleLabel: "Exam Administrator",
      name: "Alice Admin"
    };
    loginUser(adminA);

    console.log("Admin A getAdminExaminations count before creation:", getAdminExaminations().length);
    console.log("Admin A exams before creation:", getAdminExaminations().map(e => e.id));

    // Admin A creates an exam 'ALICE_EXAM'
    allExams['ALICE_EXAM'] = {
      id: 'ALICE_EXAM',
      title: 'Alice Distributed Systems',
      created_by: 'ADM101'
    };

    console.log("Admin A getAdminExaminations count after creation:", getAdminExaminations().length);
    console.log("Admin A exams:", getAdminExaminations().map(e => e.id));

    // Scenario 2: Admin A logs out
    handleSignOut();

    // Scenario 3: New Admin B registers
    const adminB = {
      id: "ADM202",
      username: "adm202",
      role: "admin",
      roleLabel: "Exam Administrator",
      name: "Bob Admin"
    };
    loginUser(adminB);

    console.log("Admin B getAdminExaminations count:", getAdminExaminations().length);
    console.log("Admin B exams:", getAdminExaminations().map(e => e.id));

    // Verify Admin B does NOT see ALICE_EXAM!
    if (getAdminExaminations().some(e => e.id === 'ALICE_EXAM')) {
      console.error("FAIL: Admin B saw Admin A exam!");
    } else {
      console.log("PASS: Admin B does NOT see Admin A exam!");
    }

    // Admin B creates an exam 'BOB_EXAM'
    allExams['BOB_EXAM'] = {
      id: 'BOB_EXAM',
      title: 'Bob Cloud Architecture',
      created_by: 'ADM202'
    };
    console.log("Admin B getAdminExaminations count after creation:", getAdminExaminations().length);
    console.log("Admin B exams:", getAdminExaminations().map(e => e.id));

    // Scenario 4: Admin B logs out, Admin A logs back in
    handleSignOut();
    loginUser(adminA);

    console.log("Admin A getAdminExaminations count after Admin B created exam:", getAdminExaminations().length);
    console.log("Admin A exams:", getAdminExaminations().map(e => e.id));
    if (getAdminExaminations().some(e => e.id === 'BOB_EXAM')) {
      console.error("FAIL: Admin A saw Admin B exam!");
    } else {
      console.log("PASS: Admin A does NOT see Admin B exam!");
    }

    // Scenario 5: Student candidate logs in
    const studentSam = {
      id: "STU1001",
      username: "stu1001",
      role: "student",
      roleLabel: "Registered Candidate",
      name: "Sam Student"
    };
    loginUser(studentSam);
    console.log("Student gets all published examinations:", getAdminExaminations().map(e => e.id));
  `;

  vm.runInContext(testScript, context);
  console.log('ALL VM ISOLATION TESTS EXECUTED SUCCESSFULLY!');

} catch (e) {
  console.error('Error during test:', e);
}
