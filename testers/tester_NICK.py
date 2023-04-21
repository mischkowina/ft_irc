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
    time.sleep(3)
    send_msg(client1, f"USER {USER1} a a {NICK1}")#welcome && MOTD
    time.sleep(3)
    #NICK - first time, 2nd user
    client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client2.connect((SERVER, PORT))
    send_msg(client2, f"PASS {PASSWORD}")#no reply, registered
    send_msg(client2, f"NICK {NICK1}")#ERR_NICKNAMEINUSE
    send_msg(client2, f"NICK {NICK2}")# no reply, success
    time.sleep(3)
    send_msg(client2, f"USER {USER2} a a {NICK2}")#welcome && MOTD
    time.sleep(3)
    #NICK with tooo many parameters
    client3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client3.connect((SERVER, PORT))
    send_msg(client3, f"PASS {PASSWORD}")#no reply
    send_msg(client3, f"NICK {NICK1} nick nick")#ERR_NICKNAMEINUSE
    send_msg(client3, f"NICK nick3 nick nick")#no reply, success
    time.sleep(3)
    send_msg(client3, f"USER user3 nick nick 2")#MOTD/Welcome
    time.sleep(3)
    time.sleep(3)
    send_msg(client3, f"QUIT")
    time.sleep(3)
    #PREP - c1 is oper on all channels
    send_msg(client1, f"JOIN #NORMAL")
    time.sleep(3)
    send_msg(client1, f"JOIN #OPER")
    time.sleep(3)
    send_msg(client1, f"JOIN #VOICE")
    time.sleep(3)
    send_msg(client1, f"JOIN #INVITE")
    time.sleep(3)
    send_msg(client2, f"JOIN #NORMAL")
    time.sleep(3)
    send_msg(client2, f"JOIN #OPER")
    time.sleep(3)
    send_msg(client1, f"MODE #OPER +o {NICK2}")
    time.sleep(3)
    send_msg(client2, f"MODE #OPER -o {NICK1}")#should work since NICK2 is operator now
    time.sleep(3)
    send_msg(client2, f"NAMES #OPER")
    time.sleep(3)
    send_msg(client2, f"JOIN #VOICE")
    time.sleep(3)
    send_msg(client1, f"MODE #VOICE +m")
    time.sleep(3)
    send_msg(client2, f"PRIVMSG #VOICE :is this on?")#ERR
    time.sleep(3)
    send_msg(client1, f"MODE #VOICE +v {NICK2}")
    time.sleep(3)
    send_msg(client2, f"PRIVMSG #VOICE :is this on now?")#should work
    time.sleep(3)
    send_msg(client2, f"NAMES #VOICE")
    time.sleep(3)
    send_msg(client1, f"MODE #INVITE +i")
    time.sleep(3)
    send_msg(client2, f"JOIN #INVITE")#ERR
    time.sleep(3)
    send_msg(client1, f"INVITE {NICK2} #INVITE")
    time.sleep(3)
    send_msg(client2, f"JOIN #INVITE")#should work
    time.sleep(3)
    send_msg(client2, f"NAMES #INVITE")
    time.sleep(3)
    send_msg(client2, f"PART #INVITE")
    #NICK change
    time.sleep(3)
    send_msg(client1, f"NAMES")
    time.sleep(3)
    send_msg(client2, f"NICK test")
    time.sleep(3)
    send_msg(client1, f"NAMES")# see if the channels are same
    time.sleep(3)
    #nick is still op
    send_msg(client2, f"MODE #OPER +o {NICK1}")
    time.sleep(3)
    send_msg(client1, f"MODE #OPER -o test")
    time.sleep(3)
    send_msg(client2, f"MODE #OPER -o {NICK1}")#err
    time.sleep(3)
    send_msg(client2, f"NAMES #OPER")
    time.sleep(3)
    #nick is still voiced
    send_msg(client2, f"PRIVMSG #VOICE :is this still on?")#should work
    time.sleep(3)
    #nick is still invited
    send_msg(client2, f"JOIN #INVITE")
    time.sleep(3)
    time.sleep(3)
    send_msg(client1, f"NAMES")
    time.sleep(3)
    #register new nick with old nicks name
    client3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client3.connect((SERVER, PORT))
    send_msg(client3, f"PASS {PASSWORD}")#no reply
    send_msg(client3, f"NICK {NICK2}")#no reply, should work
    time.sleep(3)
    send_msg(client3, f"USER b b b b")
    time.sleep(3)
    #test if he has any privileges he shouldnt have (inherited with the name)
    send_msg(client3, f"JOIN #NORMAL")#should work
    time.sleep(3)
    send_msg(client3, f"JOIN #OPER")#should work
    send_msg(client3, f"MODE #OPER -o {NICK1}")#shouldnt work
    send_msg(client3, f"NAMES #OPER")#should be only client1
    time.sleep(3)
    send_msg(client3, f"JOIN #VOICE")
    time.sleep(3)
    send_msg(client3, f"PRIVMSG #VOICE :can I also say something?")#shouldnt work
    time.sleep(3)
    send_msg(client3, f"JOIN #INVITE")#shouldnt work
    time.sleep(3)
    #kill server
    send_msg(client1, f"OPER {NICK1} operator")
    time.sleep(3)
    send_msg(client1, f"DIE")
    time.sleep(3)




if __name__ == '__main__':
    main()
