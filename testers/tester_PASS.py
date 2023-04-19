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
    send_msg(client1, f"PASS")#ERR_NEEDMOREPARAMS
    send_msg(client1, f"NICK {NICK1}")#ERR_NOTREGISTERED
    send_msg(client1, f"USER {USER1} a a {NICK1}")#ERR_NOTREGISTERED
    send_msg(client1, f"PASS wrong")#no reply, not registered
    send_msg(client1, f"NICK {NICK1}")#ERR_NOTREGISTERED
    send_msg(client1, f"USER {USER1} a a {NICK1}")#ERR_NOTREGISTERED
    send_msg(client1, f"PASS {PASSWORD}")#no reply, registered
    send_msg(client1, f"PASS {PASSWORD}")#ERR_ALREADYREGISTERED
    send_msg(client1, f"USER {USER1} a a {NICK1}")#no reply
    send_msg(client1, f"NICK {NICK1}")#welcome & motd
    time.sleep(3)  #MOT


if __name__ == '__main__':
    main()
