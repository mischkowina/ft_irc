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
#include <cerrno>

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

	bool endServer = false;
	int on = 1;
	int sockfd;
	int portnum = 6667; 		// portnum = atoi(argv[1]);
	int numClien = 200;

	struct pollfd clientsFds[numClien];

	sockaddr_in server_addr;

	// create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "ERROR opening socket" << std::endl;
		exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) == -1) {
		std::cerr << "ERROR setsockopt()" << std::endl;
		close(sockfd);
		exit(1);
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "ERROR fcntl" << std::endl;
		exit(1);
	}

	// bind the socket to an IP / port
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portnum);		// small endian -> big endian
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		std::cerr << "ERROR binding" << std::endl;
		close(sockfd);
		exit(1);
	}

	// set the socket for listening to port
	if (listen(sockfd, SOMAXCONN) == -1) {
		std::cerr << "ERROR listening" << std::endl;
		close(sockfd);
		exit(1);
	}

	// initialize poll structs
	memset(clientsFds, 0 , sizeof(clientsFds));
	for (int i = 0; i < numClien; ++i)
	{
		clientsFds[i].fd = sockfd;
		clientsFds[i].events = POLLIN;
	}

	// accept connection
	sockaddr_in	client_addr;
	socklen_t	clientSize = sizeof(client_addr);

	int		clientSocket = 0;
	char	buffer[4096];
	int		bytes_received = 0;
	int		bytes_sent = 0;

	do {
		if (poll(clientsFds, numClien, -1) == -1)
		{
			std::cerr << "ERROR at poll()" << std::endl;
			close(sockfd);
			exit(1);
		}
		
		// find fds which had returned POLLIN oposed to those stil listening
		int size = numClien;
		for (int i = 0; i < size; ++i)
		{
			// verify if able to read from client fds
			if (clientsFds[i].revents == 0)
				continue;
			if (clientsFds[i].revents != POLLIN) {
				std::cerr << "ERROR at accept()" << std::endl;
				exit(1);
			}
			if (clientsFds[i].fd == sockfd)
			{
				std::cout << "listening to the client" << std::endl;
				do {
					// clientSocket = accept(sockfd, (sockaddr *)&client_addr,	&clientSize);
					clientSocket = accept(sockfd, NULL,	NULL);
					if (clientSocket < 0) {
						if (errno != EWOULDBLOCK)
						{
							std::cerr << "ERROR accept()" << std::endl;
							exit(1);
						}
						endServer = true;
						break;
					}
					std::cout << "new connection: " << clientSocket << std::endl;
					// clientsFds[i].fd = 
				} while (clientSocket != -1);
			}
			else {
				std::cout << "read from the client sock: " << clientsFds[i].fd << std::endl;
				do {
					bzero(buffer, sizeof(buffer));

					// receive data from a client
					bytes_received = recv(clientsFds[i].fd, buffer, sizeof(buffer), 0);
					if (bytes_received == -1) {
						std::cerr << "ERROR reading from socket" << std::endl;
						close(sockfd);
						exit(1);
					}

					// return msg
					bytes_sent = send(clientsFds[i].fd, buffer, sizeof(buffer), 0);
					if (bytes_sent == -1) {
						std::cerr << "ERROR reading from socket" << std::endl;
						exit(1);
					}
					getsockname(clientSocket, (struct sockaddr *)&client_addr, &clientSize);
					std::cout << "Here is the message: " << buffer << " :from client " 
							<< inet_ntoa(client_addr.sin_addr) << std::endl;
				} while (true);
			}
		}


	} while (endServer == false);

	for (int i = 0; i < numClien; ++i)
	{
		if (clientsFds[i].fd > 0)
			close(clientsFds[i].fd);
	}
	close(sockfd);

	return 0;
}
