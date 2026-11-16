import json

with open(r'C:/Users/HP/.gemini/antigravity/brain/36133d44-b5f3-4d11-94ba-b5da053d9e65/.system_generated/logs/transcript.jsonl', 'r', encoding='utf-8') as f:
    for line in f:
        data = json.loads(line)
        if data.get('type') == 'USER_INPUT':
            content = data.get('content', '')
            print("USER:", content[:120].replace('\n', ' '))

