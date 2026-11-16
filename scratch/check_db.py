import sqlite3

conn = sqlite3.connect('online_exam.db')
c = conn.cursor()
c.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = c.fetchall()
print('Tables:', tables)
for t in tables:
    name = t[0]
    c.execute(f"SELECT count(*) FROM {name}")
    count = c.fetchone()[0]
    print(f"Table {name}: {count} rows")

print('--- Quizzes ---')
c.execute("SELECT * FROM quizzes")
for r in c.fetchall():
    print(r)

print('--- Questions count per quiz ---')
c.execute("SELECT quiz_id, count(*), avg(marks) FROM questions GROUP BY quiz_id")
for r in c.fetchall():
    print(r)

