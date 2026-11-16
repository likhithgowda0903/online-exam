import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import json
import sqlite_bridge

print("====================================================")
print("SQLITE 3 DATABASE ISOLATION TEST")
print("====================================================")

# 1. Reset / initialize
sqlite_bridge.init_db()
conn = sqlite_bridge.get_connection()
c = conn.cursor()
c.execute("DELETE FROM quizzes WHERE id IN ('WEB101', 'DBMS101', 'CPP101', 'DSA101')")
c.execute("DELETE FROM users WHERE username IN ('alice', 'bob')")
conn.commit()
conn.close()

# 2. Register Admin Alice
res1 = sqlite_bridge.register_user({
    "username": "alice",
    "password": "password123",
    "id": "ADM101",
    "name": "Alice Johnson",
    "role": "admin"
})
print("Alice registered:", res1)

# Alice has 0 exams initially
alice_initial = sqlite_bridge.get_exams("ADM101")
print("Alice initial exams:", alice_initial)
assert len(alice_initial) == 0, "Alice should have 0 exams initially"

# Alice creates 2 exams
sqlite_bridge.save_exam({
    "id": "WEB101",
    "title": "Multimedia & Web Technologies",
    "category": "Web Development",
    "durationMinutes": 45,
    "passingPercentage": 60,
    "totalMarks": 100,
    "created_by": "ADM101"
}, "ADM101")

sqlite_bridge.save_exam({
    "id": "DBMS101",
    "title": "Database Management Systems",
    "category": "Databases",
    "durationMinutes": 50,
    "passingPercentage": 65,
    "totalMarks": 100,
    "created_by": "ADM101"
}, "ADM101")

alice_exams = sqlite_bridge.get_exams("ADM101")
alice_ids = [e["id"] for e in alice_exams]
print("Alice exams after creation:", alice_ids)
assert set(alice_ids) == {"WEB101", "DBMS101"}, "Alice should have WEB101 and DBMS101"

# 3. Register Admin Bob
res2 = sqlite_bridge.register_user({
    "username": "bob",
    "password": "password123",
    "id": "ADM202",
    "name": "Bob Miller",
    "role": "admin"
})
print("Bob registered:", res2)

# Bob checks exams before creating any
bob_initial = sqlite_bridge.get_exams("ADM202")
print("Bob initial exams:", bob_initial)
assert len(bob_initial) == 0, f"Bob must NOT see Alice exams! Found: {bob_initial}"
print("✓ PASS: New Administrator Bob sees 0 exams and does NOT see Alice's exams!")

# Bob creates 2 exams
sqlite_bridge.save_exam({
    "id": "CPP101",
    "title": "C++ Programming",
    "category": "Programming Languages",
    "durationMinutes": 60,
    "passingPercentage": 70,
    "totalMarks": 100,
    "created_by": "ADM202"
}, "ADM202")

sqlite_bridge.save_exam({
    "id": "DSA101",
    "title": "Data Structures & Algorithms",
    "category": "Core CS",
    "durationMinutes": 60,
    "passingPercentage": 75,
    "totalMarks": 100,
    "created_by": "ADM202"
}, "ADM202")

bob_exams = sqlite_bridge.get_exams("ADM202")
bob_ids = [e["id"] for e in bob_exams]
print("Bob exams after creation:", bob_ids)
assert set(bob_ids) == {"CPP101", "DSA101"}, "Bob should have CPP101 and DSA101"
print("✓ PASS: Bob sees only his own exams!")

# 4. Cross-check Alice exams again
alice_recheck = sqlite_bridge.get_exams("ADM101")
alice_recheck_ids = [e["id"] for e in alice_recheck]
print("Alice exams after Bob created his:", alice_recheck_ids)
assert set(alice_recheck_ids) == {"WEB101", "DBMS101"}, "Alice should STILL only see her exams"
assert "CPP101" not in alice_recheck_ids and "DSA101" not in alice_recheck_ids
print("✓ PASS: Alice does NOT see Bob's exams!")

# 5. Security check: Bob tries to delete Alice's WEB101
hack_res = sqlite_bridge.delete_exam("WEB101", "ADM202")
print("Bob hack delete attempt result:", hack_res)
assert hack_res.get("status_code") == 403, "Bob must be blocked with 403 Forbidden"
print("✓ PASS: Unauthorized delete by Bob blocked with 403!")

# 6. Student queries all exams
all_published = sqlite_bridge.get_exams(None)
all_ids = [e["id"] for e in all_published]
print("All published exams visible to students:", all_ids)
assert "WEB101" in all_ids and "DBMS101" in all_ids and "CPP101" in all_ids and "DSA101" in all_ids
print("✓ PASS: Student can see all published exams!")

# 7. Alice legally deletes WEB101
del_res = sqlite_bridge.delete_exam("WEB101", "ADM101")
print("Alice deletes WEB101:", del_res)
assert del_res.get("success") is True
alice_after_del = sqlite_bridge.get_exams("ADM101")
assert [e["id"] for e in alice_after_del] == ["DBMS101"]
print("✓ PASS: Alice successfully deleted WEB101; DBMS101 remains.")

# Clean up test exams from DB
sqlite_bridge.delete_exam("DBMS101", "ADM101")
sqlite_bridge.delete_exam("CPP101", "ADM202")
sqlite_bridge.delete_exam("DSA101", "ADM202")

print("\n====================================================")
print("ALL SQLITE 3 ISOLATION TESTS PASSED 100%!")
print("====================================================")
