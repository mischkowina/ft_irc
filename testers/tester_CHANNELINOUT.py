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
	time.sleep(2)

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
	# Connect second client
	client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client2.connect((SERVER, PORT))
	send_msg(client2, f"PASS {PASSWORD}")#no reply, success
	send_msg(client2, f"NICK {NICK2}")#no reply, success
	send_msg(client2, f"USER 1 2 3 4")#welcome, success
	# Connect third client
	client3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client3.connect((SERVER, PORT))
	send_msg(client3, f"PASS {PASSWORD}")#no reply, success
	send_msg(client3, f"NICK client_3")#no reply, success
	send_msg(client3, f"USER 1 2 3 4")#welcome, success
	# Connect 4th client
	client4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client4.connect((SERVER, PORT))
	send_msg(client4, f"PASS {PASSWORD}")#no reply, success
	send_msg(client4, f"NICK client_4")#no reply, success
	send_msg(client4, f"USER 1 2 3 4")#welcome, success
	# Connect 5th client
	client5 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client5.connect((SERVER, PORT))
	send_msg(client5, f"PASS {PASSWORD}")#no reply, success
	send_msg(client5, f"NICK client_5")#no reply, success
	send_msg(client5, f"USER 1 2 3 4")#welcome, success
	#joining
	send_msg(client1, f"JOIN")
	send_msg(client1, f"JOIN invalid")
	send_msg(client1, f"JOIN #valid")#1
	send_msg(client1, f"JOIN &valid")#2
	send_msg(client1, f"JOIN +valid")#3
	send_msg(client1, f"JOIN #limit")#4
	send_msg(client1, f"MODE #limit +l 4")
	send_msg(client1, f"MODE +valid +l 4")
	send_msg(client1, f"MODE +valid +s")
	send_msg(client1, f"MODE +valid +p")
	send_msg(client1, f"MODE +valid +q")
	send_msg(client1, f"MODE +valid +a")
	send_msg(client1, f"MODE +valid +m")
	send_msg(client1, f"MODE +valid +t")
	send_msg(client2, f"JOIN +valid")
	send_msg(client2, f"TOPIC +valid :test_topic")
	send_msg(client1, f"LIST")
	send_msg(client1, f"TOPIC +valid :test_topic2")
	send_msg(client1, f"LIST")
	send_msg(client2, f"PART +valid")
	send_msg(client1, f"MODE +valid +b client*!*@*")
	send_msg(client1, f"MODE +valid +o client_2")
	send_msg(client1, f"NAMES")
	#try modes for +valid
	#TO-DO: multiple (valid/invalid) channels
	send_msg(client2, f"JOIN +valid,invalid,#valid,#new,#limit")
	send_msg(client1, f"NAMES")
	#channel with key
	send_msg(client1, f"MODE &valid +k key")
	send_msg(client1, f"MODE #valid +k key")
	send_msg(client3, f"JOIN #valid")
	send_msg(client3, f"JOIN #valid invalidkey")
	send_msg(client3, f"JOIN #valid key")
	send_msg(client3, f"JOIN &valid")
	send_msg(client3, f"JOIN &valid invalidkey")
	send_msg(client3, f"JOIN &valid key")
	send_msg(client3, f"JOIN #limit")
	#JOIN multiple channels, only one key (valid order)
	send_msg(client4, f"JOIN &valid,#limit wrongkey")
	send_msg(client4, f"JOIN &valid,#limit key")
	send_msg(client4, f"PART &valid,#limit")#no message
	#JOIN multiple channels, only one key (invalid order)
	send_msg(client4, f"JOIN #limit,&valid wrongkey")
	send_msg(client4, f"JOIN #limit,&valid key")
	send_msg(client4, f"PART &valid,#limit :i'm off")#message
	#JOIN multiple channels, multiple keys
	send_msg(client4, f"JOIN #valid,&valid wrongkey,wrongkey")
	send_msg(client4, f"JOIN #valid,&valid key,key")
	send_msg(client5, f"JOIN #valid,&valid key,wrongkey")
	send_msg(client5, f"JOIN #valid,&valid wrongkey,key")
	send_msg(client4, f"PART #valid,&valid")
	send_msg(client5, f"PART #valid,&valid")
	#JOIN multiple with invalid channel
	send_msg(client4, f"JOIN #valid,&valid,invalid key,key")
	send_msg(client5, f"JOIN #valid,invalid,&valid key,key")
	send_msg(client5, f"PART #valid,&valid")
	send_msg(client5, f"JOIN invalid,#valid,&valid key,key")
	#JOIN full channel
	send_msg(client4, f"JOIN #limit")
	send_msg(client5, f"JOIN #limit")#should not work
	send_msg(client1, f"NAMES")
	#JOIN too many
	send_msg(client1, f"JOIN #c5")
	send_msg(client1, f"JOIN #c6")
	send_msg(client1, f"JOIN #c7")
	send_msg(client1, f"JOIN #c8")
	send_msg(client1, f"JOIN #c9")
	send_msg(client1, f"JOIN #c10")
	send_msg(client1, f"JOIN #c11")
	send_msg(client1, f"NAMES")
	#JOIN 0
	send_msg(client1, f"JOIN 0")
	send_msg(client2, f"JOIN 0")
	send_msg(client3, f"JOIN 0")
	send_msg(client4, f"JOIN 0")
	send_msg(client5, f"JOIN 0")
	send_msg(client1, f"NAMES")
	#JOIN banned channel
	send_msg(client1, f"JOIN #ban")
	send_msg(client1, f"MODE #ban +b *2!*@*")
	send_msg(client2, f"JOIN #ban")
	send_msg(client3, f"JOIN #ban")
	send_msg(client1, f"MODE #ban +b client*!*@*")
	send_msg(client4, f"JOIN #ban")
	send_msg(client5, f"JOIN #ban")
	send_msg(client1, f"NAMES")
	#JOIN quiet channel - join message
	send_msg(client1, f"JOIN #quiet")
	send_msg(client1, f"MODE #quiet +q")
	send_msg(client2, f"JOIN #quiet")
	send_msg(client3, f"JOIN #quiet")
	send_msg(client4, f"JOIN #quiet")
	send_msg(client5, f"JOIN #quiet")
	send_msg(client1, f"MODE #quiet +o {NICK2}")
	send_msg(client3, f"PART #quiet :fucking off")
	send_msg(client2, f"KICK #quiet client_4")
	send_msg(client1, f"NAMES")
	send_msg(client5, f"NICK test")
	send_msg(client1, f"NAMES")
	send_msg(client5, f"QUIT")
	send_msg(client1, f"NAMES")
	#JOIN invite only + INVITE
	send_msg(client1, f"JOIN #invite")
	send_msg(client2, f"JOIN #invite")
	send_msg(client2, f"INVITE")# no params
	send_msg(client2, f"INVITE invalid")# one params
	send_msg(client2, f"INVITE invalid #invite")# invalid nick
	send_msg(client2, f"INVITE client_3 #invalid")# non-existing channel
	send_msg(client4, f"INVITE client_3 #invite")#by not on channel
	send_msg(client2, f"INVITE client_3 #invite")
	send_msg(client1, f"MODE #invite +i")
	send_msg(client3, f"JOIN #invite")
	send_msg(client4, f"JOIN #invite")
	send_msg(client2, f"INVITE client_4 #invite")#as normal user
	send_msg(client1, f"INVITE client_2 #invite")#already on channel
	send_msg(client4, f"AWAY :i'm away")
	send_msg(client1, f"INVITE client_4 #invite")#as chanop
	send_msg(client4, f"JOIN #invite")
	send_msg(client3, f"PART #invite")#invited coming back
	send_msg(client3, f"JOIN #invite")
	send_msg(client2, f"PART #invite :let's see if i can come back")#joined the channel before inviting, shouldnt be able to join again
	send_msg(client2, f"JOIN #invite")#shouldnt work
	send_msg(client1, f"NAMES")
	#KICK
	send_msg(client1, f"JOIN #kick")
	send_msg(client2, f"JOIN #kick")
	send_msg(client3, f"JOIN #kick")
	send_msg(client4, f"JOIN #kick")
	send_msg(client1, f"JOIN #kick2")
	send_msg(client2, f"JOIN #kick2")
	send_msg(client3, f"JOIN #kick2")
	send_msg(client4, f"JOIN #kick2")
	send_msg(client1, f"JOIN #kick3")
	send_msg(client2, f"JOIN #kick3")
	send_msg(client3, f"JOIN #kick3")
	send_msg(client1, f"KICK")
	send_msg(client1, f"KICK #kick4 client_2")#invalid channel
	send_msg(client1, f"KICK #kick invalid")#invalid client
	send_msg(client1, f"KICK #kick3 client_4")#client not on channel
	send_msg(client4, f"JOIN #kick3")
	send_msg(client2, f"KICK #kick3 client_3")#client not chanop
	send_msg(client1, f"KICK #kick3 client_4")#one channel, one nick
	send_msg(client1, f"KICK #kick3 client_2,client_3")#one channel, multiple nicks
	send_msg(client1, f"KICK #kick2,#kick client_4,client_3")#KICK with multiple channels & clients, same number
	send_msg(client3, f"JOIN #kick2,#kick")
	send_msg(client4, f"JOIN #kick2,#kick")
	send_msg(client1, f"KICK #kick2,#kick client_4,client_3,client_2")#KICK with multiple channels & clients, different number
	send_msg(client1, f"KICK #kick2,#kick5 client_4,client_3")#same number, invalid chhannel
	send_msg(client1, f"KICK #kick2,#kick invalid,client_3")#same number, invalid client
	send_msg(client2, f"JOIN 0")
	send_msg(client3, f"JOIN 0")
	send_msg(client4, f"JOIN 0")
	send_msg(client1, f"KICK #kick {NICK1}")#kick last user
	send_msg(client1, f"NAMES")
	#PART
	send_msg(client1, f"JOIN #part")
	send_msg(client2, f"JOIN #part")
	send_msg(client3, f"JOIN #part")
	send_msg(client4, f"JOIN #part")
	send_msg(client1, f"JOIN #part2")
	send_msg(client2, f"JOIN #part2")
	send_msg(client3, f"JOIN #part2")
	send_msg(client4, f"JOIN #part2")
	send_msg(client1, f"JOIN #part3")
	send_msg(client2, f"JOIN #part3")
	send_msg(client3, f"JOIN #part3")
	send_msg(client1, f"PART")#no params
	send_msg(client1, f"PART #invalid")#invalid channel
	send_msg(client4, f"PART #part3")#not on channel
	send_msg(client2, f"PART #part")#normal, one channel, no message
	send_msg(client2, f"PART #part2,#part3")#normal, multiple no message
	send_msg(client3, f"PART #part :I left one channel")
	send_msg(client3, f"PART #part2,#part3 :I left multiple channels")
	send_msg(client4, f"PART #part2,#part3,#invalid,#part :whatever")
	send_msg(client1, f"PART #part2,#part3,#invalid,#part")
	send_msg(client1, f"NAMES")#NEU
	#kill server
	send_msg(client1, f"OPER {NICK1} operator")
	time.sleep(3)
	send_msg(client1, f"DIE")


if __name__ == '__main__':
	main()
