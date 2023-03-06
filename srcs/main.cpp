#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <signal.h>
#include <poll.h>
#include <sys/select.h>

#include "server.hpp"


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

	// accept connection
	sockaddr_in	client_addr;
	socklen_t	clientSize = sizeof(client_addr);
	char		buffer[4096];

	int clientSocket;
	int bytes_received = 0;
	int bytes_sent = 0;
	
	clientSocket = accept(sockfd, (sockaddr *)&client_addr,	&clientSize);
	if (clientSocket == -1) {
		std::cerr << "ERROR on accept" << std::endl;
		exit(1);
	}
	while (true)
	{
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

	close(clientSocket);
	close(sockfd);

	return 0;
}

