import socket

local_ip = '127.0.0.1'  # IP address of the local machine
server_port = 6667  # Port of the IRC server running on the local machine
proxy_port = 7000  # Port for the IRC proxy server

proxy_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
proxy_socket.bind((local_ip, proxy_port))
proxy_socket.listen()

while True:
    client_conn, client_addr = proxy_socket.accept()
    print(f'Accepted connection from {client_addr} (client)')

    server_conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_conn.connect((local_ip, server_port))
    print(f'Connected to {local_ip}:{server_port} (server)')

    while True:
        message = client_conn.recv(1024)
        if not message:
            break
        server_conn.sendall(message)
        print(f'Sent from client to server: {message}')

        response = server_conn.recv(1024)
        if not response:
            break
        client_conn.sendall(response)
        print(f'Sent from server to client: {response}')

    server_conn.close()
    client_conn.close()

