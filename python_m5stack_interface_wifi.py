
import socket
import time


host = '192.168.56.22'
port = 22

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

s.sendall(b'Hello M5Stack\n')
s.sendall(b'this is my life\n')
s.sendall(b'this is my dog\n')
i = 0
while i<10:
    s.sendall(f"this is my cat number {i}\n".encode())
    time.sleep(1)
    i += 1
data = s.recv(1024)
print('Received', repr(data))

s.close()
