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
    #NICK - first time
    send_msg(client1, f"PASS {PASSWORD}")#no reply, registered
    send_msg(client1, f"NICK")#ERR_NONICKNAMEGIVEN
    send_msg(client1, f"NICK nickkkkkkkk")#ERR_ERRONEUSNICKNAME
    send_msg(client1, f"NICK 1nick")#ERR_ERRONEUSNICKNAME
    send_msg(client1, f"NICK &nick")#ERR_ERRONEUSNICKNAME
    send_msg(client1, f"NICK horoscope")#ERR_NICKNAMEINUSE
    send_msg(client1, f"NICK anonymous")#ERR_NICKNAMEINUSE
    send_msg(client1, f"NICK {NICK1}")#no reply, success
    send_msg(client1, f"USER {USER1} a a {NICK1}")#welcome && MOTD
    time.sleep(3)
    send_msg(client1, f"JOIN {CHANNEL}")#join replies
    send_msg(client1, f"NAMES {CHANNEL}")#names reply, client_1 in channel #42

    #NICK - first time, 2nd user
    send_msg(client1, f"PASS {PASSWORD}")#no reply, registered
    send_msg(client1, f"NICK {NICK1}")#ERR_NICKNAMEINUSE
    send_msg(client1, f"NICK {NICK2}")#ERR_NICKNAMEINUSE
    send_msg(client1, f"USER {USER2} a a {NICK2}")#welcome && MOTD
    time.sleep(3)
    send_msg(client1, f"JOIN {CHANNEL}")#join replies
    send_msg(client1, f"NAMES {CHANNEL}")#names reply, client_1 & client_2in channel #42

    #NICK change


if __name__ == '__main__':
    main()
