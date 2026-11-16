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

const domMocks = {
  document: {
    getElementById: (id) => elements[id] || makeMockEl(),
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
vm.runInContext(scriptContent, context);

const test = `
  const admin = {
    id: "ADM101",
    username: "adm101",
    role: "admin",
    roleLabel: "Exam Administrator",
    name: "Dr. Smith"
  };
  loginUser(admin);

  console.log("Before creation, adminExams:", getAdminExaminations());

  // Call handleCreateQuizSubmit
  handleCreateQuizSubmit();

  console.log("After creation, allExams['CS102']:", allExams['CS102']);
  console.log("After creation, getAdminExaminations():", getAdminExaminations().map(e => e.id));

  // Check rendered HTML in appViewport
  const container = document.getElementById('appViewport');
  console.log("Rendered HTML length:", container.innerHTML.length);
  console.log("Contains CS102:", container.innerHTML.includes('CS102'));
  console.log("Contains Database Systems:", container.innerHTML.includes('Database Systems'));

  // Switch to quizzes tab
  switchAdminTab('quizzes');
  console.log("On quizzes tab, contains CS102:", container.innerHTML.includes('CS102'));
  console.log("On quizzes tab, contains Database Systems:", container.innerHTML.includes('Database Systems'));
`;

vm.runInContext(test, context);

