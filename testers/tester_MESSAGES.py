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
	time.sleep(3)

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
	#connect second client
	client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client2.connect((SERVER, PORT))
	send_msg(client2, f"PASS {PASSWORD}")#no reply, success
	send_msg(client2, f"USER 1 2 3 4")#no reply, success
	send_msg(client2, f"NICK {NICK2}")#welcome, success
	#connect third client
	client3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client3.connect((SERVER, PORT))
	send_msg(client3, f"PASS {PASSWORD}")#no reply, success
	send_msg(client3, f"USER 1 2 3 4")#no reply, success
	send_msg(client3, f"NICK client_3")#welcome, success
	#names for all
	send_msg(client1, f"NAMES")#success
	#privmsg && notice
	#no recipient
	send_msg(client1, f"PRIVMSG")#success
	send_msg(client1, f"NOTICE")#success
	#no message
	send_msg(client1, f"PRIVMSG {NICK2}")#success
	send_msg(client1, f"NOTICE {NICK2}")#success
	#invlid receiver
	send_msg(client1, f"PRIVMSG invalid :test message")#success
	send_msg(client1, f"NOTICE invalid :test message")#success
	#mixed receivers, one invalid: client2(away), channel, client3
	send_msg(client2, f"AWAY :i'm away")
	send_msg(client1, f"PRIVMSG {NICK2},invalid,client_3 :test message")#success
	send_msg(client1, f"NOTICE {NICK2},invalid,client_3 :test message")#success
	# normal channel
	send_msg(client1, f"JOIN #normal")
	send_msg(client2, f"JOIN #normal")
	send_msg(client3, f"JOIN #normal")
	send_msg(client2, f"PRIVMSG #normal,client_3 :test normal channel")
	send_msg(client2, f"NOTICE #normal,client_3 :test normal channel")
	# moderated channel as voiced user && unvoiced user
	send_msg(client1, f"JOIN #moderated")
	send_msg(client2, f"JOIN #moderated")
	send_msg(client1, f"MODE #moderated +m")
	send_msg(client1, f"MODE #moderated +v client_2")
	send_msg(client3, f"PRIVMSG #moderated :test from outside")
	send_msg(client3, f"NOTICE #moderated :test from outside")
	send_msg(client3, f"JOIN #moderated")
	send_msg(client1, f"PRIVMSG #moderated :test from chanop")
	send_msg(client1, f"NOTICE #moderated :test from chanop")
	send_msg(client2, f"PRIVMSG #moderated :test from voiced")
	send_msg(client2, f"NOTICE #moderated :test from voiced")
	send_msg(client3, f"PRIVMSG #moderated :test from normal user")
	send_msg(client3, f"NOTICE #moderated :test from normal user")
	# channel without outside messages as outsider && insider
	send_msg(client1, f"JOIN #nooutside")
	send_msg(client2, f"JOIN #nooutside")
	send_msg(client1, f"MODE #nooutside +n")
	send_msg(client1, f"PRIVMSG #nooutside :test from chanop")
	send_msg(client1, f"NOTICE #nooutside :test from chanop")
	send_msg(client2, f"PRIVMSG #nooutside :test from inside")
	send_msg(client2, f"NOTICE #nooutside :test from inside")
	send_msg(client3, f"PRIVMSG #nooutside :test from outside")
	send_msg(client3, f"NOTICE #nooutside :test from outside")
	# as banned user
	send_msg(client1, f"JOIN #ban")
	send_msg(client2, f"JOIN #ban")
	send_msg(client1, f"MODE #ban +b *3!*@*")
	send_msg(client3, f"JOIN #ban")#should not work
	send_msg(client1, f"PRIVMSG #ban :test from chanop")
	send_msg(client1, f"NOTICE #ban :test from chanop")
	send_msg(client2, f"PRIVMSG #ban :test from non-banned")
	send_msg(client2, f"NOTICE #ban :test from non-banned")
	send_msg(client3, f"PRIVMSG #ban :test from banned")
	send_msg(client3, f"NOTICE #ban :test from banned")
	send_msg(client1, f"MODE #ban -b *3!*@*")
	send_msg(client3, f"PRIVMSG #ban :test from banned")
	send_msg(client3, f"NOTICE #ban :test from banned")
	# no more messages after PARTNG
	send_msg(client3, f"PART #normal")
	send_msg(client3, f"PRIVMSG #normal :test normal channel")
	send_msg(client3, f"NOTICE #normal :test normal channel")
	# to horoscope
	send_msg(client3, f"PRIVMSG horoscope :CAPRICORN")
	send_msg(client3, f"NOTICE horoscope :CAPRICORN")
	#kill server
	send_msg(client1, f"OPER {NICK1} operator")
	time.sleep(3)
	send_msg(client1, f"DIE")


if __name__ == '__main__':
	main()
