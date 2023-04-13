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


def main():
    # Connect first client
    client1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client1.connect((SERVER, PORT))
    send_msg(client1, f"PASS {PASSWORD}")
    send_msg(client1, f"USER {USER1} a a {NICK1}")
    send_msg(client1, f"NICK {NICK1}")
    time.sleep(3)  # Wait for server to acknowledge connection
    send_msg(client1, f"JOIN {CHANNEL}")
    send_msg(client1, f"PRIVMSG {CHANNEL} :Hello from {NICK1}!")
    recv_msg(client1)  # Wait for server response

    # Connect second client
    client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client2.connect((SERVER, PORT))
    send_msg(client2, f"PASS {PASSWORD}")
    send_msg(client2, f"USER {USER2} b b {NICK2}")
    send_msg(client2, f"NICK {NICK2}")
    time.sleep(3)  # Wait for server to acknowledge connection
    send_msg(client2, f"JOIN {CHANNEL}")
    send_msg(client2, f"PRIVMSG {CHANNEL} :Hello from {NICK2}!")
    recv_msg(client2)  # Wait for server response
    #testing
    join_limit(client2, f"JOIN #test")


if __name__ == '__main__':
    main()

