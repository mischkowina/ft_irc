import socket
import time

SERVER = 'localhost'
PORT = 6667
CHANNEL = '#42'
PASSWORD = 'password'
NICK1 = 'client_1'
NICK2 = 'client_2'
USER1 = 'userName_1'
USER2 = 'userName_2'

def send_msg(sock, msg):
    sock.send(f"{msg}\r\n".encode())

def recv_msg(sock):
    data = sock.recv(1024).decode().strip()
    if data:
        print(f"< {data}")
    return data

# tests
def join_limit(sock, msg):
	i = 0
	while i < 12:
		sock.send(f"{msg}\r\n".encode())
		i += 1

def gen_testing(sock):
    sock.send(f"JOIN #42\r\n".encode())
    sock.send(f"JOIN &42\r\n".encode())
    sock.send(f"JOIN +42\r\n".encode())
    sock.send(f"JOIN $42\r\n".encode())
    sock.send(f"JOIN 42\r\n".encode())
    sock.send(f"INVALID CMD\r\n".encode())
    sock.send(f"JOIN #42,#twotwo pass\r\n".encode())
    sock.send(f"LIST\r\n".encode())


def main():
    # Connect first client
    client1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client1.connect((SERVER, PORT))
    send_msg(client1, f"PASS {PASSWORD}")#no reply, success
    send_msg(client1, f"NICK {NICK1}")#no reply, success
    time.sleep(3)
    send_msg(client1, f"USER 1 2 3 4")#welcome, success
    time.sleep(3)
    send_msg(client1, f"JOIN #NORMAL")#success
    time.sleep(3)
    send_msg(client1, f"JOIN #ANONYM")#success
    time.sleep(3)
    send_msg(client1, f"MODE #ANONYM +a")#make anonymous
    time.sleep(3)
    #second client, no message
    client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client2.connect((SERVER, PORT))
    send_msg(client2, f"PASS {PASSWORD}")#no reply, success
    send_msg(client2, f"USER 1 2 3 4")#no reply, success
    time.sleep(3)
    send_msg(client2, f"NICK {NICK2}")#welcome, success
    time.sleep(3)
    send_msg(client2, f"JOIN #NORMAL")#success
    time.sleep(3)
    send_msg(client2, f"JOIN #ANONYM")#success
    time.sleep(3)
    send_msg(client1, f"NAMES")#success
    time.sleep(3)
    #third client, message
    client3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client3.connect((SERVER, PORT))
    send_msg(client3, f"PASS {PASSWORD}")#no reply, success
    send_msg(client3, f"USER 1 2 3 4")#no reply, success
    time.sleep(3)
    send_msg(client3, f"NICK client_3")#welcome, success
    time.sleep(3)
    send_msg(client3, f"JOIN #NORMAL")#success
    time.sleep(3)
    send_msg(client3, f"JOIN #ANONYM")#success
    time.sleep(3)
    send_msg(client1, f"NAMES")#success
    time.sleep(3)
    #QUIT connection for both
    send_msg(client2, f"QUIT")#success
    time.sleep(3)
    send_msg(client3, f"QUIT :fuck you all")#success
    time.sleep(3)
    #kill server
    send_msg(client1, f"OPER {NICK1} operator")
    time.sleep(3)
    send_msg(client1, f"DIE")


if __name__ == '__main__':
    main()
