import json
import re

with open('scratch/seed_oop_exam.py', 'r', encoding='utf-8') as f:
    text = f.read()

# Load OOP_QUESTIONS and OOP_EXAM from seed_oop_exam.py
import sys
sys.path.append('scratch')
from seed_oop_exam import OOP_EXAM, OOP_QUESTIONS

# Construct the JS string for defaultOOPQuestions
js_questions = []
for q in OOP_QUESTIONS:
    js_questions.append({
        "id": q["num"],
        "text": f"{q['num']}. {q['text']}",
        "options": q["options"],
        "correct": q["correct"],
        "marks": q["marks"],
        "explanation": q["explanation"]
    })

js_questions_str = json.dumps(js_questions, indent=6)

js_block = f"""    const defaultOOPQuestions = {js_questions_str};

    const masterQuestionPools = {{
      'OOP': defaultOOPQuestions
    }};
    const allExams = {{
      'OOP': {{
        id: 'OOP',
        title: 'Object-Oriented Programming (OOPs)',
        category: 'C++ Programming',
        totalQuestions: 20,
        durationMinutes: 15,
        passingPercentage: 60,
        totalMarks: 100,
        questions: []
      }}
    }};"""

print("Generated JS block length:", len(js_block))

