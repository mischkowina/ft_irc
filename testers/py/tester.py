import socket
import time
import IRC_tests  # import the new module


def main():
    client1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client1.connect((SERVER, PORT))
    send_msg(client1, f"PASS {PASSWORD}")
    send_msg(client1, f"NICK {NICK1}")
    send_msg(client1, f"USER {USER1} 0 * :{NICK1}")
    time.sleep(2)  # Wait for server to acknowledge connection
    irc_tests.test_join_channel(client1)  # call the new test function
    send_msg(client1, f"PRIVMSG {CHANNEL} :Hello from {NICK1}!")


