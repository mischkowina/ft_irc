import socket
import time

SERVER = 'irc.freenode.net'
PORT = 6667
CHANNEL = '#test'
PASSWORD = 'password'
NICK1 = 'client_1'
NICK2 = 'client_2'
USER1 = 'userName_1'
USER2 = 'userName_2'

def send_msg(sock, msg):
    sock.send(f"{msg}\r\n".encode())

def main():
    # Connect first client
    client1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client1.connect((SERVER, PORT))
    send_msg(client1, f"PASS {PASSWORD}")
    send_msg(client1, f"NICK {NICK1}")
    send_msg(client1, f"USER {USER1} 0 * :{NICK1}")
    time.sleep(2)  # Wait for server to acknowledge connection
    send_msg(client1, f"JOIN {CHANNEL}")
    send_msg(client1, f"PRIVMSG {CHANNEL} :Hello from {NICK1}!")

    # Connect second client
    client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client2.connect((SERVER, PORT))
    send_msg(client2, f"PASS {PASSWORD}")
    send_msg(client2, f"NICK {NICK2}")
    send_msg(client2, f"USER {USER2} 0 * :{NICK2}")
    time.sleep(2)  # Wait for server to acknowledge connection
    send_msg(client2, f"JOIN {CHANNEL}")
    send_msg(client2, f"PRIVMSG {CHANNEL} :Hello from {NICK2}!")

if __name__ == '__main__':
    main()

