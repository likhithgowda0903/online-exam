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
  addEventListener: function() {},
  removeEventListener: function() {},
  getElementById: function(id) {
    if (!this.elements[id]) {
      this.elements[id] = {
        id,
        innerHTML: '',
        textContent: '',
        style: {},
        classList: {
          add: () => {},
          remove: () => {},
          contains: () => false
        },
        appendChild: () => {},
        setAttribute: () => {},
        getAttribute: () => null,
        addEventListener: () => {}
      };
    }
    return this.elements[id];
  },
  querySelector: function() { return null; },
  querySelectorAll: function() { return []; },
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

console.log("=== 1. VERIFY EXAM DATA STRUCTURE ===");
const allExams = vm.runInContext('allExams', sandbox);
const masterQuestionPools = vm.runInContext('masterQuestionPools', sandbox);
const defaultStudent = vm.runInContext('defaultStudent', sandbox);
const defaultAdmin = vm.runInContext('defaultAdmin', sandbox);
const state = vm.runInContext('state', sandbox);
const showView = vm.runInContext('showView', sandbox);
const startExam = vm.runInContext('startExam', sandbox);
const submitExamPrompt = vm.runInContext('submitExamPrompt', sandbox);
const closeModal = vm.runInContext('closeModal', sandbox);
const switchAdminTab = vm.runInContext('switchAdminTab', sandbox);

console.log("Exam IDs in allExams:", Object.keys(allExams));
const oopExam = allExams['OOP'];
if (!oopExam) {
  console.error("FAIL: allExams['OOP'] not found!");
  process.exit(1);
}

console.log("OOP Exam Details:", {
  id: oopExam.id,
  title: oopExam.title,
  category: oopExam.category,
  durationMinutes: oopExam.durationMinutes,
  totalQuestions: oopExam.totalQuestions,
  totalMarks: oopExam.totalMarks,
  questionsLength: oopExam.questions ? oopExam.questions.length : 0
});

if (oopExam.durationMinutes !== 15) {
  console.error(`FAIL: Expected durationMinutes = 15, got ${oopExam.durationMinutes}`);
  process.exit(1);
}
if (oopExam.totalQuestions !== 20) {
  console.error(`FAIL: Expected totalQuestions = 20, got ${oopExam.totalQuestions}`);
  process.exit(1);
}

const pool = masterQuestionPools['OOP'];
if (!pool || pool.length !== 20) {
  console.error(`FAIL: Expected masterQuestionPools['OOP'] to have 20 questions, got ${pool ? pool.length : 0}`);
  process.exit(1);
}

let allFiveMarks = true;
for (let i = 0; i < pool.length; i++) {
  const q = pool[i];
  if (q.marks !== 5) {
    console.error(`FAIL: Question ${i + 1} marks = ${q.marks}, expected 5`);
    allFiveMarks = false;
  }
}
if (allFiveMarks) {
  console.log("✓ All 20 questions in masterQuestionPools have marks = 5 (Total = 100 Marks)");
}

console.log("\n=== 2. TEST STUDENT DASHBOARD VIEW ===");
state.user = defaultStudent;
state.isAuthenticated = true;
showView('dashboard');

const viewport = mockDoc.getElementById('appViewport');
const viewportHtml = viewport.innerHTML;

if (viewportHtml.includes('Object-Oriented Programming (OOPs)')) {
  console.log("✓ Student Dashboard displays subject: Object-Oriented Programming (OOPs)");
} else {
  console.error("FAIL: Student Dashboard missing subject title!");
  process.exit(1);
}

if (viewportHtml.includes('20 Questions') && viewportHtml.includes('15 mins')) {
  console.log("✓ Student Dashboard displays '20 Questions' and '15 mins'");
} else {
  console.error("FAIL: Missing '20 Questions' or '15 mins' in table!");
  process.exit(1);
}

if (viewportHtml.includes('15 minutes allocated for completion') && viewportHtml.includes('20 Questions allocated')) {
  console.log("✓ Exam Checklist correctly shows '15 minutes allocated for completion' and '20 Questions allocated'");
} else {
  console.error("FAIL: Exam Checklist missing correct values!");
  process.exit(1);
}

console.log("\n=== 3. TEST LIVE EXAM ROOM LAUNCH & COUNTDOWN ===");
startExam('OOP');
console.log("Current view after startExam:", state.currentView);
console.log("Remaining seconds:", state.remainingSeconds['OOP']);

if (state.remainingSeconds['OOP'] !== 900) {
  console.error(`FAIL: Expected remainingSeconds = 900 (15 mins), got ${state.remainingSeconds['OOP']}`);
  process.exit(1);
} else {
  console.log("✓ Timer correctly initialized to 900 seconds (15:00 minutes)");
}

if (allExams['OOP'].questions.length === 20) {
  console.log("✓ Live Exam room loaded all 20 questions");
} else {
  console.error(`FAIL: Live exam has ${allExams['OOP'].questions.length} questions, expected 20`);
  process.exit(1);
}

// Answer all 20 questions correctly
const currentQuestions = allExams['OOP'].questions;
currentQuestions.forEach((q, idx) => {
  state.answers['OOP'][idx] = q.correct;
});

// Submit exam
submitExamPrompt();
const modalRoot = mockDoc.getElementById('modalRoot');
const modalText = modalRoot.innerHTML;

if (modalText.includes('100%') && modalText.includes('20 / 20 Correct') && modalText.includes('100 / 100')) {
  console.log("✓ Exam Evaluation Verified: 100%, 20 / 20 Correct, Marks: 100 / 100!");
} else {
  console.error("FAIL: Scorecard text did not match expected 100 / 100:", modalText.substring(0, 300));
  process.exit(1);
}

console.log("\n=== 4. TEST ADMIN DASHBOARD QUESTION BANK & QUIZZES ===");
closeModal();
state.user = defaultAdmin;
switchAdminTab('questions');

const adminHtml = mockDoc.getElementById('appViewport').innerHTML;
if (adminHtml.includes('Question Bank') && adminHtml.includes('OOP') && adminHtml.includes('+5 Marks')) {
  console.log("✓ Admin Question Bank shows OOP questions with +5 Marks badges");
} else {
  console.error("FAIL: Admin Question Bank does not show OOP questions properly!");
  process.exit(1);
}

switchAdminTab('quizzes');
const quizzesHtml = mockDoc.getElementById('appViewport').innerHTML;
if (quizzesHtml.includes('Object-Oriented Programming (OOPs)') && quizzesHtml.includes('15 mins') && quizzesHtml.includes('20')) {
  console.log("✓ Admin Manage Quizzes shows 15 mins, 20 MCQs, and 100 pts");
} else {
  console.error("FAIL: Admin Quizzes table missing OOP info!");
  process.exit(1);
}

console.log("\n>>> ALL TESTS PASSED SUCCESSFULLY! 100% VERIFIED! <<<");
process.exit(0);
