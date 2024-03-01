#!/usr/bin/python3

import datetime
import cgi
import sys

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("</head>")
print("<body>")
print("<h1>", datetime.datetime.strftime(datetime.datetime.now(), "%H:%M:%S"), "</h1>")
print("</body>")
print("</html>")

# Закрываем стандартный вывод, чтобы веб-сервер понял, что ответ завершен
sys.stdout.close()