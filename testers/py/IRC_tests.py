def test_join_channel(client_socket):
    send_msg(client_socket, f"JOIN {CHANNEL}")
    response = client_socket.recv(2048).decode()
    assert "JOIN" in response, f"JOIN failed. Server response: {response}"



