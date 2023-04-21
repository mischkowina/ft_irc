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
def test_user_limit(sock, msg, val):
	i = 0
	while i < val:
		sock.send(f"{msg} #{i}\r\n".encode())
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

def key_pass1(sock):
    sock.send(f"JOIN #segF\r\n".encode())
    sock.send(f"MODE #segF +k pass\r\n".encode())

def key_pass2(sock):
    sock.send(f"JOIN #segF\r\n".encode())

def main():
    # Connect first client
    client1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client1.connect((SERVER, PORT))
    send_msg(client1, f"PASS {PASSWORD}")
    send_msg(client1, f"USER {USER1} A A {NICK1}")
    send_msg(client1, f"NICK {NICK1}")
    time.sleep(3)  # Wait for server to acknowledge connection
    send_msg(client1, f"JOIN {CHANNEL}")
    send_msg(client1, f"PRIVMSG {CHANNEL} :Hello from {NICK1}!")
    recv_msg(client1)  # Wait for server response

    # Connect second client
    client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client2.connect((SERVER, PORT))
    send_msg(client2, f"PASS {PASSWORD}")
    send_msg(client2, f"USER {USER2} B B {NICK2}")
    send_msg(client2, f"NICK {NICK2}")
    time.sleep(3)  # Wait for server to acknowledge connection
    send_msg(client2, f"JOIN {CHANNEL}")
    send_msg(client2, f"PRIVMSG {CHANNEL} :Hello from {NICK2}!")
    recv_msg(client2)  # Wait for server response
   
    # testing
    # test_user_limit(client2,f"JOIN", 14)
    # test_user_limit(client2,f"PART", 7)
    # test_user_limit(client2,f"JOIN", 7)
    # send_msg(client2, f"JOIN #OneTooMany")
    # send_msg(client2, f"NAMES")
    #gen_testing(client2)

    key_pass1(client1)
    key_pass2(client2)

if __name__ == '__main__':
    main()
