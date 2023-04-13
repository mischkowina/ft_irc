import socket
import time
import unittest

SERVER = 'localhost'
PORT = 6667
CHANNEL = '#test'
PASSWORD = 'password'
NICK1 = 'client_1'
NICK2 = 'client_2'
USER1 = 'userName_1'
USER2 = 'userName_2'


class TestIRCServer(unittest.TestCase):
    def setUp(self):
        self.client1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def tearDown(self):
        self.client1.close()
        self.client2.close()

    def test_connection(self):
        self.client1.connect((SERVER, PORT))
        self.client2.connect((SERVER, PORT))
        time.sleep(2)  # Wait for server to acknowledge connection
        self.assertEqual(2, len(socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect_ex((SERVER, PORT))))
    
    def test_join(self):
        self.client1.connect((SERVER, PORT))
        send_msg(self.client1, f"PASS {PASSWORD}")
        send_msg(self.client1, f"NICK {NICK1}")
        send_msg(self.client1, f"USER {USER1} 0 * :{NICK1}")
        time.sleep(2)  # Wait for server to acknowledge connection
        send_msg(self.client1, f"JOIN {CHANNEL}")
        self.assertIn(f":{NICK1}!{USER1}@{SERVER} JOIN {CHANNEL}", self.client1.recv(1024).decode())

        self.client2.connect((SERVER, PORT))
        send_msg(self.client2, f"PASS {PASSWORD}")
        send_msg(self.client2, f"NICK {NICK2}")
        send_msg(self.client2, f"USER {USER2} 0 * :{NICK2}")
        time.sleep(2)  # Wait for server to acknowledge connection
        send_msg(self.client2, f"JOIN {CHANNEL}")
        self.assertIn(f":{NICK2}!{USER2}@{SERVER} JOIN {CHANNEL}", self.client1.recv(1024).decode())

    def test_send_message(self):
        self.client1.connect((SERVER, PORT))
        send_msg(self.client1, f"PASS {PASSWORD}")
        send_msg(self.client1, f"NICK {NICK1}")
        send_msg(self.client1, f"USER {USER1} 0 * :{NICK1}")
        time.sleep(2)  # Wait for server to acknowledge connection
        send_msg(self.client1, f"JOIN {CHANNEL}")
        send_msg(self.client1, f"PRIVMSG {CHANNEL} :Hello from {NICK1}!")
        self.assertIn(f":{NICK1}!{USER1}@{SERVER} PRIVMSG {CHANNEL} :Hello from {NICK1}!", self.client1.recv(1024).decode())

        self.client2.connect((SERVER, PORT))
        send_msg(self.client2, f"PASS {PASSWORD}")
        send_msg(self.client2, f"NICK {NICK2}")
        send_msg(self.client2, f"USER {USER2} 0 * :{NICK2}")
        time.sleep(2)  # Wait for server to acknowledge connection
        send_msg(self.client2, f"JOIN {CHANNEL}")
        self.assertIn(f":{NICK1}!{USER1}@{SERVER} PRIVMSG {CHANNEL} :Hello from {NICK1}!", self.client2.recv(1024


