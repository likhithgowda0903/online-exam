with open('index.html', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, l in enumerate(lines):
    if "activeTab === 'students' ?" in l or "activeTab === 'students' ?" in l.replace('"', "'"):
        for j in range(i, min(i+50, len(lines))):
            print(lines[j], end='')
        break
