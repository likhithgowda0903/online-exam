"""
SQLite3 Bridge for Online Examination Portal Dev Server
Provides seamless database operations for online_exam.db
"""

import sys
import json
import sqlite3
import os

DB_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "online_exam.db")

def get_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    conn = get_connection()
    c = conn.cursor()
    # Ensure created_by column exists in quizzes
    c.execute("PRAGMA table_info(quizzes)")
    cols = [r["name"] for r in c.fetchall()]
    if "created_by" not in cols:
        c.execute("ALTER TABLE quizzes ADD COLUMN created_by TEXT")
    conn.commit()
    conn.close()

def are_admins_same(owner1, owner2):
    if not owner1 or not owner2:
        return True
    if owner1.strip().lower() == owner2.strip().lower():
        return True
    try:
        conn = get_connection()
        c = conn.cursor()
        c.execute("""
            SELECT username, specific_id, email FROM users
            WHERE (LOWER(username) = LOWER(?) OR LOWER(specific_id) = LOWER(?) OR LOWER(email) = LOWER(?))
              AND (LOWER(username) = LOWER(?) OR LOWER(specific_id) = LOWER(?) OR LOWER(email) = LOWER(?))
        """, (owner1, owner1, owner1, owner2, owner2, owner2))
        row = c.fetchone()
        conn.close()
        return row is not None
    except Exception:
        return False

def get_status():
    return {
        "status": "online",
        "engine": "SQLite 3",
        "database_file": "online_exam.db",
        "version": sqlite3.sqlite_version
    }

def get_students():
    conn = get_connection()
    c = conn.cursor()
    c.execute("SELECT id, username, full_name as name, email, specific_id as rollId, role FROM users WHERE LOWER(role) = 'student'")
    rows = [dict(r) for r in c.fetchall()]
    conn.close()
    return rows

def get_admins():
    conn = get_connection()
    c = conn.cursor()
    c.execute("SELECT id, username, password, full_name as name, email, specific_id as rollId, role FROM users WHERE LOWER(role) = 'admin'")
    rows = [dict(r) for r in c.fetchall()]
    conn.close()
    return rows

def register_user(data):
    conn = get_connection()
    c = conn.cursor()
    username = data.get("username", "").strip()
    password = data.get("password", "").strip()
    name = data.get("name", "").strip()
    email = data.get("email", "").strip() or None
    role = data.get("role", "student").strip()
    specific_id = data.get("id", "").strip()

    # Check if user already exists by username or specific_id
    c.execute("SELECT id FROM users WHERE LOWER(username) = LOWER(?) OR (specific_id IS NOT NULL AND LOWER(specific_id) = LOWER(?))", (username, specific_id))
    row = c.fetchone()
    if row:
        c.execute("""
            UPDATE users SET password = ?, full_name = ?, email = ?, role = ?, specific_id = ?
            WHERE id = ?
        """, (password, name, email, role, specific_id, row["id"]))
    else:
        c.execute("""
            INSERT INTO users (username, password, full_name, email, role, specific_id, created_at)
            VALUES (?, ?, ?, ?, ?, ?, datetime('now'))
        """, (username, password, name, email, role, specific_id))
    conn.commit()
    conn.close()
    return {"success": True, "message": "User registered successfully"}

def get_exams(admin_id=None):
    conn = get_connection()
    c = conn.cursor()
    # Always return all examinations from quizzes so both Student UI and Administrator Console have full visibility
    c.execute("""
        SELECT id, title, category, duration_minutes as durationMinutes,
               pass_percentage as passingPercentage, total_marks as totalMarks,
               created_by, created_at
        FROM quizzes
        ORDER BY created_at DESC
    """)
    rows = [dict(r) for r in c.fetchall()]
    conn.close()
    return rows

def save_exam(data, admin_id=None):
    conn = get_connection()
    c = conn.cursor()
    exam_id = data.get("id", "").strip().upper()
    title = data.get("title", "").strip()
    cat = data.get("category", "Computer Science").strip()
    dur = int(data.get("durationMinutes", 45))
    pass_pct = float(data.get("passingPercentage", 60.0))
    marks = int(data.get("totalMarks", 100))
    creator = data.get("created_by") or admin_id or ""

    c.execute("SELECT id, created_by FROM quizzes WHERE UPPER(id) = UPPER(?)", (exam_id,))
    row = c.fetchone()
    if row:
        existing_owner = row["created_by"] or ""
        final_creator = existing_owner if existing_owner else creator
        c.execute("""
            UPDATE quizzes SET title = ?, category = ?, duration_minutes = ?, pass_percentage = ?, total_marks = ?, created_by = ?
            WHERE UPPER(id) = UPPER(?)
        """, (title, cat, dur, pass_pct, marks, final_creator, exam_id))
    else:
        c.execute("""
            INSERT INTO quizzes (id, title, category, duration_minutes, pass_percentage, total_marks, created_by, created_at)
            VALUES (?, ?, ?, ?, ?, ?, ?, datetime('now'))
        """, (exam_id, title, cat, dur, pass_pct, marks, creator))
    conn.commit()
    conn.close()
    return {"success": True, "id": exam_id, "created_by": creator}

def delete_exam(exam_id, admin_id=None):
    conn = get_connection()
    c = conn.cursor()
    c.execute("SELECT id, created_by FROM quizzes WHERE UPPER(id) = UPPER(?)", (exam_id,))
    row = c.fetchone()
    if not row:
        conn.close()
        return {"error": "Examination not found", "status_code": 404}
    c.execute("DELETE FROM quizzes WHERE UPPER(id) = UPPER(?)", (exam_id,))
    c.execute("DELETE FROM questions WHERE UPPER(quiz_id) = UPPER(?)", (exam_id,))
    conn.commit()
    conn.close()
    return {"success": True, "message": "Examination and associated questions deleted successfully"}

def get_questions(quiz_id=None):
    conn = get_connection()
    c = conn.cursor()
    if quiz_id and quiz_id.strip():
        c.execute("""
            SELECT id, quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation
            FROM questions
            WHERE UPPER(quiz_id) = UPPER(?)
            ORDER BY id ASC
        """, (quiz_id.strip(),))
        rows = c.fetchall()
        questions = []
        for r in rows:
            questions.append({
                "id": r["id"],
                "text": r["question_text"],
                "options": [r["option_a"] or "", r["option_b"] or "", r["option_c"] or "", r["option_d"] or ""],
                "correct": int(r["correct_option"] or 0),
                "marks": int(r["marks"] or 5),
                "explanation": r["explanation"] or ""
            })
        conn.close()
        return questions
    else:
        c.execute("""
            SELECT id, quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation
            FROM questions
            ORDER BY id ASC
        """)
        rows = c.fetchall()
        pools = {}
        for r in rows:
            qid = r["quiz_id"]
            if qid not in pools:
                pools[qid] = []
            pools[qid].append({
                "id": r["id"],
                "text": r["question_text"],
                "options": [r["option_a"] or "", r["option_b"] or "", r["option_c"] or "", r["option_d"] or ""],
                "correct": int(r["correct_option"] or 0),
                "marks": int(r["marks"] or 5),
                "explanation": r["explanation"] or ""
            })
        conn.close()
        return pools

def save_question(data):
    conn = get_connection()
    c = conn.cursor()
    quiz_id = (data.get("quiz_id") or data.get("exam_id") or "").strip().upper()
    text = (data.get("text") or data.get("question_text") or "").strip()
    options = data.get("options") or ["", "", "", ""]
    opt_a = options[0] if len(options) > 0 else (data.get("option_a") or "")
    opt_b = options[1] if len(options) > 1 else (data.get("option_b") or "")
    opt_c = options[2] if len(options) > 2 else (data.get("option_c") or "")
    opt_d = options[3] if len(options) > 3 else (data.get("option_d") or "")
    correct = int(data.get("correct") if "correct" in data else data.get("correct_option", 0))
    marks = int(data.get("marks", 5))
    explanation = data.get("explanation", "")

    # Check if duplicate question exists for this quiz
    c.execute("""
        SELECT id FROM questions
        WHERE UPPER(quiz_id) = UPPER(?) AND LOWER(TRIM(question_text)) = LOWER(?)
    """, (quiz_id, text))
    row = c.fetchone()
    if row:
        c.execute("""
            UPDATE questions
            SET option_a = ?, option_b = ?, option_c = ?, option_d = ?, correct_option = ?, marks = ?, explanation = ?
            WHERE id = ?
        """, (opt_a, opt_b, opt_c, opt_d, correct, marks, explanation, row["id"]))
        q_id = row["id"]
    else:
        c.execute("""
            INSERT INTO questions (quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, (quiz_id, text, opt_a, opt_b, opt_c, opt_d, correct, marks, explanation))
        q_id = c.lastrowid

    # Update total questions count in quizzes table
    c.execute("SELECT COUNT(*) as cnt FROM questions WHERE UPPER(quiz_id) = UPPER(?)", (quiz_id,))
    cnt_row = c.fetchone()
    if cnt_row:
        c.execute("UPDATE quizzes SET total_questions = ? WHERE UPPER(id) = UPPER(?)", (cnt_row["cnt"], quiz_id))

    conn.commit()
    conn.close()
    return {"success": True, "id": q_id, "quiz_id": quiz_id}

def delete_question(question_id, quiz_id=None):
    conn = get_connection()
    c = conn.cursor()
    c.execute("DELETE FROM questions WHERE id = ?", (question_id,))
    if quiz_id:
        c.execute("SELECT COUNT(*) as cnt FROM questions WHERE UPPER(quiz_id) = UPPER(?)", (quiz_id,))
        cnt_row = c.fetchone()
        if cnt_row:
            c.execute("UPDATE quizzes SET total_questions = ? WHERE UPPER(id) = UPPER(?)", (cnt_row["cnt"], quiz_id))
    conn.commit()
    conn.close()
    return {"success": True}

if __name__ == "__main__":
    init_db()
    if len(sys.argv) < 2:
        print(json.dumps(get_status()))
        sys.exit(0)

    cmd = sys.argv[1]
    if cmd == "status":
        print(json.dumps(get_status()))
    elif cmd == "students":
        print(json.dumps(get_students()))
    elif cmd == "admins":
        print(json.dumps(get_admins()))
    elif cmd == "exams":
        admin = sys.argv[2] if len(sys.argv) > 2 else None
        print(json.dumps(get_exams(admin)))
    elif cmd == "register":
        input_data = json.loads(sys.stdin.read())
        print(json.dumps(register_user(input_data)))
    elif cmd == "save_exam":
        admin = sys.argv[2] if len(sys.argv) > 2 else None
        input_data = json.loads(sys.stdin.read())
        print(json.dumps(save_exam(input_data, admin)))
    elif cmd == "delete_exam":
        exam_id = sys.argv[2] if len(sys.argv) > 2 else ""
        admin = sys.argv[3] if len(sys.argv) > 3 else None
        print(json.dumps(delete_exam(exam_id, admin)))
    elif cmd == "questions":
        quiz_id = sys.argv[2] if len(sys.argv) > 2 else None
        print(json.dumps(get_questions(quiz_id)))
    elif cmd == "save_question":
        input_data = json.loads(sys.stdin.read())
        print(json.dumps(save_question(input_data)))
    elif cmd == "delete_question":
        q_id = sys.argv[2] if len(sys.argv) > 2 else 0
        quiz_id = sys.argv[3] if len(sys.argv) > 3 else None
        print(json.dumps(delete_question(q_id, quiz_id)))
    else:
        print(json.dumps({"error": f"Unknown command: {cmd}"}))
