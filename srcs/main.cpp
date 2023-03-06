
#include "../include/irc.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	// if (argc != 3)
	// 		std::cerr << Correct use: ./ircserv <port> <password> << std::endl;

	int sockfd;
	int portnum = 6667; 		// portnum = atoi(argv[1]);
	
	sockaddr_in server_addr;

	// create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "ERROR opening socket" << std::endl;
		exit(1);
	}

	// bind the socket to an IP / port
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portnum);		// small endian -> big endian
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		std::cerr << "ERROR binding" << std::endl;
		exit(1);
	}

	// set the socket for listening port
	if (listen(sockfd, SOMAXCONN) == -1) {
		std::cerr << "ERROR listening" << std::endl;
		exit(1);
	}

	std::cout << "Server running." << std::endl;

	char			buffer[4096];
	int				bytes_received = 0;
	int				bytes_sent = 0;
	ft::ClientMap	client_map;//typedef in irc.hpp
	
	while (true)
	{
		//loop to accept all new connections
		sockaddr_in	client_addr;//temporary variable
		socklen_t	clientSize = sizeof(client_addr);//temporary variable
		int 		clientSocket;//temporary variable

		while (true)
		{
			clientSocket = accept(sockfd, (sockaddr *)&client_addr,	&clientSize);
			if (clientSocket == -1)
				break ;
			Client	newClient(clientSocket);
			newClient.setAddress(&client_addr);
			std::pair<ft::ClientMap::iterator, bool> insert_return = client_map.insert(make_pair(newClient.getAddress(), newClient));
			if (insert_return.second == false)
				std::cout << "Connection request failed: duplicate key." << std::endl;
			else
				std::cout << "Client succesfully connected from " << newClient.getAddress() << ". Total number of connected clients: " << client_map.size() << std::endl;
		}

		//create struct pollfd with the appropriate size and populate it with the socket fds of all connected clients
		struct pollfd pollfds[client_map.size()];
		int i = 0;
		for (ft::ClientMap::iterator it = client_map.begin(); it != client_map.end(); it++, i++)
		{
			pollfds[i].fd = it->second.getSocket();
			pollfds[i].events = POLLIN;
		}
		
		bzero(buffer, sizeof(buffer));
		int n = fcntl(clientSocket, F_SETFL, O_NONBLOCK);
		if (n == -1) {
			std::cerr << "ERROR on fcntl" << std::endl;
			exit(1);
		}

		// receive data from a client
		bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
		if (bytes_received == -1) {
			std::cerr << "ERROR reading from socket" << std::endl;
			close(sockfd);
			exit(1);
		}

		// forward data to other clients
		bytes_sent = send(clientSocket, buffer, sizeof(buffer), 0);
		if (bytes_sent == -1)
			exit(1);

		getsockname(clientSocket, (struct sockaddr *)&client_addr, &clientSize);
		std::cout << "Here is the message: " << buffer << " :from client " 
				<< inet_ntoa(client_addr.sin_addr) << std::endl;

		
		

	}

	// close(clientSocket);
	close(sockfd);

	return 0;
}

