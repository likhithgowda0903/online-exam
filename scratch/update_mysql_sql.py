import sys
sys.path.append('scratch')
from seed_oop_exam import OOP_EXAM, OOP_QUESTIONS

def generate_mysql_insert():
    lines = []
    lines.append("\n-- ========================================================")
    lines.append("-- 4.1 SEED EXAMINATIONS: OBJECT-ORIENTED PROGRAMMING (OOPS)")
    lines.append("-- ========================================================")
    lines.append(f"""INSERT INTO `examinations` (`id`, `title`, `description`, `category`, `duration_minutes`, `passing_percentage`, `total_marks`, `total_questions`, `created_at`)
VALUES ('{OOP_EXAM["id"]}', '{OOP_EXAM["title"]}', 'C++ Object-Oriented Programming 20-MCQ Practice Set', '{OOP_EXAM["category"]}', {OOP_EXAM["duration_minutes"]}, {int(OOP_EXAM["pass_percentage"])}, {OOP_EXAM["total_marks"]}, {len(OOP_QUESTIONS)}, NOW())
ON DUPLICATE KEY UPDATE `title` = VALUES(`title`), `duration_minutes` = VALUES(`duration_minutes`), `total_marks` = VALUES(`total_marks`), `total_questions` = VALUES(`total_questions`);\n""")
    
    lines.append("-- ========================================================")
    lines.append("-- 5.1 SEED 20 OOP QUESTIONS (5 MARKS EACH, 15 MINS EXAM)")
    lines.append("-- ========================================================")
    lines.append("DELETE FROM `questions` WHERE `quiz_id` = 'OOP';")
    for q in OOP_QUESTIONS:
        clean_text = f"{q['num']}. {q['text']}".replace("'", "''")
        clean_a = q['options'][0].replace("'", "''")
        clean_b = q['options'][1].replace("'", "''")
        clean_c = q['options'][2].replace("'", "''")
        clean_d = q['options'][3].replace("'", "''")
        clean_expl = q['explanation'].replace("'", "''")
        lines.append(f"""INSERT INTO `questions` (`quiz_id`, `question_text`, `option_a`, `option_b`, `option_c`, `option_d`, `correct_option`, `marks`, `explanation`)
VALUES ('OOP', '{clean_text}', '{clean_a}', '{clean_b}', '{clean_c}', '{clean_d}', {q['correct']}, {q['marks']}, '{clean_expl}');""")
    
    lines.append("\n")
    return "\n".join(lines)

sql_snippet = generate_mysql_insert()

for path in ['online_exam_mysql.sql', 'web-portal/online_exam_mysql.sql']:
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check if already present
    if "SEED EXAMINATIONS: OBJECT-ORIENTED PROGRAMMING (OOPS)" in content:
        # replace
        pos = content.find("-- 4.1 SEED EXAMINATIONS: OBJECT-ORIENTED PROGRAMMING (OOPS)")
        end_pos = content.find("-- VERIFICATION QUERIES FOR MYSQL WORKBENCH")
        if pos != -1 and end_pos != -1:
            content = content[:pos-4] + sql_snippet + content[end_pos:]
    else:
        # insert before VERIFICATION QUERIES
        end_pos = content.find("-- ========================================================\n-- VERIFICATION QUERIES FOR MYSQL WORKBENCH")
        if end_pos != -1:
            content = content[:end_pos] + sql_snippet + content[end_pos:]
        else:
            content += sql_snippet

    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Updated {path}")

