import sqlite3
import json
import os

OOP_EXAM = {
    "id": "OOP",
    "title": "Object-Oriented Programming (OOPs)",
    "category": "C++ Programming",
    "duration_minutes": 15,
    "pass_percentage": 60.0,
    "total_marks": 100
}

OOP_QUESTIONS = [
    {
        "num": 1,
        "text": "Which OOP concept bundles data and the functions that operate on it into one unit?",
        "options": ["Inheritance", "Encapsulation", "Polymorphism", "Compilation"],
        "correct": 1, # B
        "marks": 5,
        "explanation": "Encapsulation combines data and related member functions within a class."
    },
    {
        "num": 2,
        "text": "Which keyword is used to declare a class in C++?",
        "options": ["object", "define", "class", "new"],
        "correct": 2, # C
        "marks": 5,
        "explanation": "The class keyword introduces a class definition."
    },
    {
        "num": 3,
        "text": "An object is best described as:",
        "options": ["An instance of a class", "A function inside a class", "A header file", "A data type keyword"],
        "correct": 0, # A
        "marks": 5,
        "explanation": "A class is a blueprint; an object is an instance created from it."
    },
    {
        "num": 4,
        "text": "Which access specifier makes class members accessible only within the class and its friends?",
        "options": ["public", "protected", "private", "global"],
        "correct": 2, # C
        "marks": 5,
        "explanation": "Private members are accessible to the class itself and its friend functions/classes."
    },
    {
        "num": 5,
        "text": "What is the main purpose of a constructor?",
        "options": ["To destroy an object", "To initialize an object", "To overload an operator", "To inherit a class"],
        "correct": 1, # B
        "marks": 5,
        "explanation": "A constructor runs during object creation and initializes the object."
    },
    {
        "num": 6,
        "text": "Which special member function is called when an object's lifetime ends?",
        "options": ["Constructor", "Destructor", "Main function", "Friend function"],
        "correct": 1, # B
        "marks": 5,
        "explanation": "A destructor performs cleanup when an object is destroyed."
    },
    {
        "num": 7,
        "text": "Which symbol appears before a destructor's class name?",
        "options": ["#", "!", "~", "&"],
        "correct": 2, # C
        "marks": 5,
        "explanation": "A destructor is declared with a tilde followed by the class name."
    },
    {
        "num": 8,
        "text": "Which concept allows a derived class to acquire accessible members of a base class?",
        "options": ["Abstraction", "Inheritance", "Encapsulation", "Overloading"],
        "correct": 1, # B
        "marks": 5,
        "explanation": "Inheritance lets a derived class reuse and extend a base class."
    },
    {
        "num": 9,
        "text": "Which inheritance type has one derived class inheriting from two or more base classes?",
        "options": ["Single", "Multilevel", "Multiple", "Hierarchical"],
        "correct": 2, # C
        "marks": 5,
        "explanation": "Multiple inheritance means one class has more than one direct base class."
    },
    {
        "num": 10,
        "text": "Which concept means one interface can represent different underlying forms?",
        "options": ["Polymorphism", "Encapsulation", "Compilation", "Tokenization"],
        "correct": 0, # A
        "marks": 5,
        "explanation": "Polymorphism allows the same interface to produce different behavior."
    },
    {
        "num": 11,
        "text": "Function overloading is an example of which type of polymorphism?",
        "options": ["Runtime", "Compile-time", "Dynamic allocation", "Inheritance-only"],
        "correct": 1, # B
        "marks": 5,
        "explanation": "The compiler selects an overloaded function based on its arguments."
    },
    {
        "num": 12,
        "text": "Which keyword enables dynamic dispatch for a member function?",
        "options": ["static", "friend", "virtual", "inline"],
        "correct": 2, # C
        "marks": 5,
        "explanation": "Virtual functions support runtime polymorphism when called through a base pointer or reference."
    },
    {
        "num": 13,
        "text": "What is function overloading?",
        "options": [
            "Same function name with different parameter lists",
            "Same function body in every class",
            "A function with no parameters only",
            "Defining two main functions"
        ],
        "correct": 0, # A
        "marks": 5,
        "explanation": "Overloaded functions share a name but differ in their parameter lists."
    },
    {
        "num": 14,
        "text": "What is the purpose of a friend function?",
        "options": [
            "It automatically creates objects",
            "It can access a class's private and protected members",
            "It prevents inheritance",
            "It replaces constructors"
        ],
        "correct": 1, # B
        "marks": 5,
        "explanation": "A friend function is not a member, but the class grants it special access."
    },
    {
        "num": 15,
        "text": "Which operator accesses a member through an object pointer?",
        "options": [".", "::", "->", "&"],
        "correct": 2, # C
        "marks": 5,
        "explanation": "The arrow operator accesses a member through a pointer."
    },
    {
        "num": 16,
        "text": "What does the this pointer refer to in a non-static member function?",
        "options": ["The base class", "The current object", "The next object", "The global object"],
        "correct": 1, # B
        "marks": 5,
        "explanation": "this points to the object on which the member function was called."
    },
    {
        "num": 17,
        "text": "What is operator overloading?",
        "options": [
            "Creating a completely new operator symbol",
            "Giving an existing operator behavior for a user-defined type",
            "Changing operator precedence",
            "Removing an operator"
        ],
        "correct": 1, # B
        "marks": 5,
        "explanation": "Operator overloading defines how supported operators work with class or enum types."
    },
    {
        "num": 18,
        "text": "Which statement about a static data member is correct?",
        "options": [
            "Every object has a separate copy",
            "It is shared by all objects of the class",
            "It can only be local to main",
            "It must be private"
        ],
        "correct": 1, # B
        "marks": 5,
        "explanation": "A static data member belongs to the class and is shared by its objects."
    },
    {
        "num": 19,
        "text": "What is abstraction in OOP?",
        "options": [
            "Showing essential features while hiding implementation details",
            "Copying all data into another object",
            "Making every member public",
            "Creating multiple constructors"
        ],
        "correct": 0, # A
        "marks": 5,
        "explanation": "Abstraction exposes what an object does while hiding unnecessary implementation detail."
    },
    {
        "num": 20,
        "text": "Which statement about a pure virtual function is correct?",
        "options": [
            "It is declared using = 0",
            "It must be static",
            "It cannot appear in a base class",
            "It is always private"
        ],
        "correct": 0, # A
        "marks": 5,
        "explanation": "A pure virtual function is declared with = 0 and makes the class abstract."
    }
]

db_paths = ['online_exam.db', 'web-portal/online_exam.db', 'desktop-app/online_exam.db']

for db_path in db_paths:
    if not os.path.exists(db_path):
        continue
    print(f"Updating database: {db_path}...")
    conn = sqlite3.connect(db_path)
    c = conn.cursor()
    
    # 1. Upsert quiz
    c.execute("""
        INSERT INTO quizzes (id, title, category, duration_minutes, pass_percentage, total_marks, created_at)
        VALUES (?, ?, ?, ?, ?, ?, datetime('now'))
        ON CONFLICT(id) DO UPDATE SET
            title = excluded.title,
            category = excluded.category,
            duration_minutes = excluded.duration_minutes,
            pass_percentage = excluded.pass_percentage,
            total_marks = excluded.total_marks;
    """, (
        OOP_EXAM["id"],
        OOP_EXAM["title"],
        OOP_EXAM["category"],
        OOP_EXAM["duration_minutes"],
        OOP_EXAM["pass_percentage"],
        OOP_EXAM["total_marks"]
    ))
    
    # 2. Delete existing questions for this quiz
    c.execute("DELETE FROM questions WHERE quiz_id = ?", (OOP_EXAM["id"],))
    
    # 3. Insert all 20 questions
    for q in OOP_QUESTIONS:
        c.execute("""
            INSERT INTO questions (quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            OOP_EXAM["id"],
            f"{q['num']}. {q['text']}",
            q["options"][0],
            q["options"][1],
            q["options"][2],
            q["options"][3],
            q["correct"],
            q["marks"],
            q["explanation"]
        ))
        
    conn.commit()
    
    # Verify counts
    c.execute("SELECT count(*) FROM quizzes WHERE id = ?", (OOP_EXAM["id"],))
    q_count = c.fetchone()[0]
    c.execute("SELECT count(*), avg(marks) FROM questions WHERE quiz_id = ?", (OOP_EXAM["id"],))
    row = c.fetchone()
    print(f"  -> Quizzes: {q_count}, Questions: {row[0]}, Avg Marks: {row[1]}")
    conn.close()

print("All databases successfully updated!")
