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
  newQuizTitle: { value: 'Web Technologies' },
  newQuizCat: { value: 'Computer Science' },
  newQuizCode: { value: 'WEB101' },
  newQuizDur: { value: '45' },
  newQuizPass: { value: '60' },
  newQuizMarks: { value: '100' },
  newQExamSelect: { value: 'WEB101' },
  newQText: { value: 'What does HTML stand for?' },
  newQOpt0: { value: 'Hyper Text Markup Language' },
  newQOpt1: { value: 'High Text Machine Language' },
  newQOpt2: { value: 'Hyperlink Text Module' },
  newQOpt3: { value: 'None of the above' },
  newQCorrect: { value: '0' },
  newQMarks: { value: '5' },
  newQExplanation: { value: 'Standard definition.' },
  appViewport: makeMockEl(),
  modalRoot: makeMockEl(),
  sidebarNavList: makeMockEl()
};

// Real persistent localStorage simulation
const persistentStorage = {
  store: {},
  getItem: function(key) {
    const val = this.store[key];
    console.log(`[STORAGE GET] ${key} => ${val ? (val.length > 60 ? val.substring(0, 60) + '...' : val) : 'null'}`);
    return val !== undefined ? val : null;
  },
  setItem: function(key, val) {
    console.log(`[STORAGE SET] ${key} => ${val.length > 60 ? val.substring(0, 60) + '...' : val}`);
    this.store[key] = String(val);
  },
  removeItem: function(key) {
    console.log(`[STORAGE REMOVE] ${key}`);
    delete this.store[key];
  }
};

console.log('==============================================');
console.log('STEP 1: FIRST SESSION (LOGIN, CREATE, ADD QUESTION)');
console.log('==============================================');

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
    localStorage: persistentStorage,
    console: console,
    setTimeout: setTimeout,
    clearTimeout: clearTimeout,
    setInterval: setInterval,
    clearInterval: clearInterval,
    fetch: () => Promise.resolve({ ok: false })
  };

  const context = vm.createContext(domMocks);
  vm.runInContext(scriptContent, context);

  const testSession1 = `
    const admin = {
      id: "ADM101",
      username: "adm101",
      role: "admin",
      roleLabel: "Exam Administrator",
      name: "Alice Admin"
    };
    loginUser(admin);

    console.log("Admin initial exams:", getAdminExaminations().map(e => e.id));

    // 1. Admin creates exam WEB101
    handleCreateQuizSubmit();
    console.log("After create, allExams['WEB101']:", allExams['WEB101']);
    console.log("After create, adminExams:", getAdminExaminations().map(e => e.id));

    // 2. Admin adds question to WEB101
    handleAddQuestionSubmit();
    console.log("After add question, masterQuestionPools['WEB101'] count:", (masterQuestionPools['WEB101'] || []).length);
  `;

  vm.runInContext(testSession1, context);
}

console.log('\n==============================================');
console.log('STEP 2: PAGE REFRESH (NEW SCRIPT RUN, RELOGIN)');
console.log('==============================================');

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
    localStorage: persistentStorage,
    console: console,
    setTimeout: setTimeout,
    clearTimeout: clearTimeout,
    setInterval: setInterval,
    clearInterval: clearInterval,
    fetch: () => Promise.resolve({ ok: false })
  };

  const context = vm.createContext(domMocks);
  vm.runInContext(scriptContent, context);

  const testSession2 = `
    console.log("On refresh startup allExams keys:", Object.keys(allExams));
    console.log("On refresh startup masterQuestionPools keys:", Object.keys(masterQuestionPools));

    const admin = {
      id: "ADM101",
      username: "adm101",
      role: "admin",
      roleLabel: "Exam Administrator",
      name: "Alice Admin"
    };
    loginUser(admin);

    const adminExams = getAdminExaminations();
    console.log("After relogin, adminExams count:", adminExams.length);
    console.log("After relogin, adminExams:", adminExams.map(e => ({ id: e.id, title: e.title, created_by: e.created_by })));
    console.log("After relogin, masterQuestionPools['WEB101'] count:", (masterQuestionPools['WEB101'] || []).length);

    // Render dashboard and inspect container HTML
    const container = document.getElementById('appViewport');
    renderAdminDashboard(container);
    console.log("Dashboard contains WEB101:", container.innerHTML.includes('WEB101'));
    console.log("Dashboard contains Web Technologies:", container.innerHTML.includes('Web Technologies'));
  `;

  vm.runInContext(testSession2, context);
}

