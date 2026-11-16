const fs = require('fs');
const vm = require('vm');

const html = fs.readFileSync('index.html', 'utf8');

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

const elements = {
  newQuizTitle: { value: 'Database Systems' },
  newQuizCat: { value: 'Computer Science' },
  newQuizCode: { value: 'CS102' },
  newQuizDur: { value: '45' },
  newQuizPass: { value: '60' },
  newQuizMarks: { value: '100' },
  appViewport: makeMockEl(),
  modalRoot: makeMockEl(),
  sidebarNavList: makeMockEl()
};

const sharedLocalStorage = {
  store: {},
  getItem: function(key) { return this.store[key] || null; },
  setItem: function(key, val) { this.store[key] = String(val); },
  removeItem: function(key) { delete this.store[key]; }
};

// SESSION 1: Admin creates exam CS102
{
  const domMocks = {
    document: {
      getElementById: (id) => elements[id] || makeMockEl(),
      querySelector: () => makeMockEl(),
      querySelectorAll: () => [makeMockEl()],
      addEventListener: () => {},
      createElement: () => makeMockEl(),
      body: makeMockEl()
    },
    localStorage: sharedLocalStorage,
    console: console,
    setTimeout: setTimeout,
    clearTimeout: clearTimeout,
    setInterval: setInterval,
    clearInterval: clearInterval,
    fetch: () => Promise.resolve({ ok: false })
  };

  const context = vm.createContext(domMocks);
  vm.runInContext(scriptContent, context);

  const test1 = `
    const admin = {
      id: "ADM101",
      username: "adm101",
      role: "admin",
      roleLabel: "Exam Administrator",
      name: "Dr. Smith"
    };
    loginUser(admin);
    handleCreateQuizSubmit();
    console.log("Session 1 created_by:", allExams['CS102'].created_by);
  `;
  vm.runInContext(test1, context);
}

// SESSION 2: Page reload / refresh
console.log("\n--- SESSION 2: PAGE RELOAD ---");
{
  const domMocks = {
    document: {
      getElementById: (id) => elements[id] || makeMockEl(),
      querySelector: () => makeMockEl(),
      querySelectorAll: () => [makeMockEl()],
      addEventListener: () => {},
      createElement: () => makeMockEl(),
      body: makeMockEl()
    },
    localStorage: sharedLocalStorage,
    console: console,
    setTimeout: setTimeout,
    clearTimeout: clearTimeout,
    setInterval: setInterval,
    clearInterval: clearInterval,
    fetch: () => Promise.resolve({ ok: false })
  };

  const context = vm.createContext(domMocks);
  vm.runInContext(scriptContent, context);

  const test2 = `
    console.log("Session 2 allExams keys on startup:", Object.keys(allExams));
    console.log("Session 2 allExams['CS102'] on startup:", allExams['CS102']);

    const admin = {
      id: "ADM101",
      username: "adm101",
      role: "admin",
      roleLabel: "Exam Administrator",
      name: "Dr. Smith"
    };
    loginUser(admin);

    console.log("Session 2 adminExams count:", getAdminExaminations().length);
    console.log("Session 2 adminExams:", getAdminExaminations());
  `;
  vm.runInContext(test2, context);
}

