with open('index.html', 'rb') as f1, open('web-portal/index.html', 'rb') as f2:
    print('index.html matches web-portal/index.html:', f1.read() == f2.read())

with open('online_exam_mysql.sql', 'rb') as f1, open('web-portal/online_exam_mysql.sql', 'rb') as f2:
    print('online_exam_mysql.sql matches web-portal/online_exam_mysql.sql:', f1.read() == f2.read())

