import re

with open('index.html', 'r', encoding='utf-8') as f:
    text = f.read()

matches = re.findall(r'<input[^>]+id=[\'"]auth[^\'"]+[\'"][^>]*>', text)
for m in matches:
    print(m)

