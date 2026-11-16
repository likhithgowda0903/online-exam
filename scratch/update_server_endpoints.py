import sys

endpoints_code = """        # API: Register User into SQLite 3
        if path == '/api/register':
            content_len = int(self.headers.get('Content-Length', 0))
            post_body = self.rfile.read(content_len).decode('utf-8')
            try:
                payload = json.loads(post_body)
                conn = get_db()
                c = conn.cursor()
                c.execute(\"\"\"
                    INSERT OR REPLACE INTO users (username, password, full_name, email, role, specific_id, created_at)
                    VALUES (?, ?, ?, ?, ?, ?, datetime('now'))
                \"\"\", (
                    payload['username'].strip().lower(),
                    payload['password'],
                    payload['name'],
                    payload.get('email', '').strip().lower(),
                    payload.get('role', 'student'),
                    payload.get('id', payload['username'].upper())
                ))
                conn.commit()
                conn.close()
                return self.send_json({'success': True})
            except Exception as e:
                return self.send_json({'success': False, 'error': str(e)}, 400)

        # API: Verify Student Account for Password Recovery
        if path == '/api/verify-student':
            content_len = int(self.headers.get('Content-Length', 0))
            post_body = self.rfile.read(content_len).decode('utf-8')
            try:
                payload = json.loads(post_body)
                ident = str(payload.get('identifier', '')).strip().lower()
                email = str(payload.get('email', '')).strip().lower()
                conn = get_db()
                c = conn.cursor()
                c.execute(\"\"\"
                    SELECT id, username, specific_id, full_name, email 
                    FROM users 
                    WHERE role = 'student' 
                      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
                      AND LOWER(email) = ?
                \"\"\", (ident, ident, email))
                row = c.fetchone()
                conn.close()
                if row:
                    return self.send_json({
                        'verified': True,
                        'username': row['username'],
                        'student_id': row['specific_id'],
                        'full_name': row['full_name']
                    })
                else:
                    return self.send_json({'verified': False, 'error': 'No matching account found'}, 404)
            except Exception as e:
                return self.send_json({'verified': False, 'error': str(e)}, 400)

        # API: Reset Student Password in SQLite 3
        if path == '/api/reset-password':
            content_len = int(self.headers.get('Content-Length', 0))
            post_body = self.rfile.read(content_len).decode('utf-8')
            try:
                payload = json.loads(post_body)
                ident = str(payload.get('identifier', '')).strip().lower()
                new_pwd = str(payload.get('new_password', ''))
                if not ident or not new_pwd:
                    return self.send_json({'success': False, 'error': 'Missing parameters'}, 400)
                conn = get_db()
                c = conn.cursor()
                c.execute(\"\"\"
                    UPDATE users 
                    SET password = ? 
                    WHERE role = 'student' 
                      AND (LOWER(username) = ? OR LOWER(specific_id) = ?)
                \"\"\", (new_pwd, ident, ident))
                conn.commit()
                updated = c.rowcount
                conn.close()
                return self.send_json({'success': True, 'rows_updated': updated})
            except Exception as e:
                return self.send_json({'success': False, 'error': str(e)}, 500)
"""

for sfile in ['start_server.py', 'web-portal/start_server.py']:
    with open(sfile, 'r', encoding='utf-8') as f:
        content = f.read()
    
    if '/api/verify-student' in content:
        print(f"Endpoints already present in {sfile}")
        continue
    
    # Insert right before 'if path == \'/api/open-database\':' in do_POST
    pos = content.find("        if path == '/api/open-database':")
    # There are two 'if path == \'/api/open-database\':' (one in do_GET, one in do_POST)
    # Find the second one (in do_POST)
    first_pos = content.find("        if path == '/api/open-database':")
    second_pos = content.find("        if path == '/api/open-database':", first_pos + 1)
    
    if second_pos != -1:
        content = content[:second_pos] + endpoints_code + "\n" + content[second_pos:]
        with open(sfile, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Successfully added recovery endpoints to {sfile}")
    else:
        print(f"Could not find insert position in {sfile}")

