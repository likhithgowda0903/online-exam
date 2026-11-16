import sqlite3

for db_path in ['online_exam.db', 'web-portal/online_exam.db']:
    conn = sqlite3.connect(db_path)
    c = conn.cursor()
    c.execute("SELECT id, username, specific_id, full_name, role, email FROM users")
    rows = c.fetchall()
    print(db_path, "Users:", rows)
    c.execute("SELECT id, user_id, quiz_id, score, total_questions, percentage, completed_at FROM results")
    res = c.fetchall()
    print(db_path, "Results:", res)
    conn.close()

