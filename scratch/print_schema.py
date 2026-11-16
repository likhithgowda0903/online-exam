import sqlite3
conn = sqlite3.connect('online_exam.db')
c = conn.cursor()
c.execute("SELECT sql FROM sqlite_master WHERE name='users'")
print(c.fetchone()[0])
conn.close()

