#!/bin/env python

import socketserver
import http.server
import os
import sys

# 通过参数，指定运行的目录
dir = sys.argv[1]
os.chdir(dir)

# 通过参数，指定运行的端口
port = int(sys.argv[2])

# 杀死之前的进程
# command = 'taskkill /F /IM t.exe' 
# 这个不弹出黑框
# os.popen(command)

class ThreadingSimpleServer(socketserver.ThreadingMixIn,
                   http.server.HTTPServer):
    pass



#myIP = socket.gethostbyname_ex(socket.gethostname())[-1][0]
# 获取内网IP
myIP = [a for a in os.popen('route print').readlines() if ' 0.0.0.0 ' in a][0].split()[-2]
# port = 8866

server = ThreadingSimpleServer((myIP, port), http.server.SimpleHTTPRequestHandler)
print("OK")
print(myIP+":"+str(port))

try:
    while 1:
        sys.stdout.flush()
        server.handle_request()
except KeyboardInterrupt:
    print("Finished")
