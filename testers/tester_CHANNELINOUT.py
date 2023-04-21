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
	send_msg(client1, f"USER 1 2 3 4")#welcome, success
	#JOIN
	#JOIN without params
	#JOIN 0
	#JOIN new channel with valid/invalid name
	#JOIN some existing and some new channels
	#JOIN multiple channels
	#JOIN multiple channels, only one key
	#JOIN multiple channels, multiple keys
	#JOIN invalid channel
	#JOIN full channel
	#JOIN password channel with / wo password
	#JOIN banned channel
	#JOIN too many channels
	#JOIN quiet channel - join message
	#JOIN invite only
	#INVITE without params
	#INVITE invalid nick
	#INVITE invalid channel 
	#INVITE by not on channel
	#INVITE to invite only as chanop/normal user
	#INVITE to away user
	#INVITE client already on channel
	#KICK without params
	#KICK with only one channel and one nick
	#KICK with only one channel and multiple nicks
	#KICK with multiple channels & clients, same number
	#KICK with multiple channels & clients, but not same number
	#KICK invalid channel
	#KICK client not on channel
	#KICK as non chanop
	#KICK from quiet channel
	#KICK last user from channel (myself??)
	#PART without parameters
	#PART invalid channel
	#PART not on channel
	#PART one channel
	#PART multiple channels
	#PART one channel wo message
	#PART multiple channels with message
	#kill server
	send_msg(client1, f"OPER {NICK1} operator")
	time.sleep(3)
	send_msg(client1, f"DIE")


if __name__ == '__main__':
	main()
