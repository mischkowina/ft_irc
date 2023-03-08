
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
		perror(NULL);
		exit(1);
	}

	// set the socket for listening port
	if (listen(sockfd, SOMAXCONN) == -1) {
		std::cerr << "ERROR listening" << std::endl;
		exit(1);
	}

	//set listening socket to non-blocking
	int fcntl_return = fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (fcntl_return == -1)
	{
		std::cerr << "ERROR on fcntl" << std::endl;
		exit(1);
	}

	std::cout << "Server running." << std::endl;

	// char			buffer[4096];
	// int				bytes_received = 0;
	// int				bytes_sent = 0;
	ft::ClientMap	client_map;//typedef in irc.hpp
	
	while (true)
	{
		// loop to accept all new connections
		// sockaddr_in	client_addr;//temporary variable / gets reused
		// socklen_t	clientSize = sizeof(client_addr);//temporary variable / gets reused

		// while (true)
		// {
		// 	int	clientSocket = accept(sockfd, (sockaddr *)&client_addr,	&clientSize);
		// 	if (clientSocket == -1)
		// 		break ;
		// 	std::cout << "clientSock: " << clientSocket << std::endl;
		// 	Client	newClient(clientSocket);
		// 	newClient.setAddress(&client_addr);
		// 	std::pair<ft::ClientMap::iterator, bool> insert_return = client_map.insert(make_pair(newClient.getAddress(), newClient));
		// 	if (insert_return.second == false)
		// 		std::cout << "Connection request failed: duplicate key." << std::endl;
		// 	else
		// 		std::cout << "Client succesfully connected from " << newClient.getAddress() << ". Total number of connected clients: " << client_map.size() << std::endl;
		// }

		
		//create struct pollfd[] with the appropriate size (number of connected clients + 1 for the listening socket)
		struct pollfd pollfds[client_map.size() + 1];

		//initalize the first struct pollfd[0] to the listening socket
		bzero(&pollfds[0], sizeof(pollfds[0]));
		pollfds[0].fd = sockfd;
		pollfds[0].events = POLLIN;

		//populate the rest of the struct pollfd[] with the client socket descriptors
		int i = 1;
		for (ft::ClientMap::iterator it = client_map.begin(); it != client_map.end(); it++, i++)
		{
			bzero(&pollfds[i], sizeof(pollfds[i]));
			pollfds[i].fd = it->second.getSocket();
			pollfds[i].events = POLLIN;
		}

		//poll
		int pollreturn = poll(pollfds, client_map.size() + 1, 0);
		if (pollreturn < 0)
		{
			std::cerr << "ERROR on poll" << std::endl;
			exit(1);//tbd if we want to exit or just continue running the server??
		}
		if (pollreturn == 0)
			continue ;

		int current_size = client_map.size() + 1;
		for (int i = 0; i < current_size; i++)
		{
			std::cout << "POLLFDS[" << i << "].revents: " << pollfds[i].revents << std::endl;
			// if (pollfds[i].revents != POLLIN) //skip all pollfds without an event
			// 	continue ;
			// else if (pollfds[i].revents != POLLIN)//TBD if we want to protect against other events?
			// {
			// 	std::cerr << "ERROR: unexpected revent on fd " << pollfds[i].fd << std::endl;
			// 	exit(1);
			// }
			if (pollfds[i].revents & POLLIN) //POLLIN event occured!
			{
				if (i == 0) //first fd, listening socket --> if this has an event, there is a new connection pending that has to be accpeted
				{
					sockaddr_in	client_addr;
					socklen_t	clientSize = sizeof(client_addr);
					int 		clientSocket;

					//accept all new pending connections
					while (true)
					{
						clientSocket = accept(sockfd, (sockaddr *)&client_addr,	&clientSize); 
						if (clientSocket == -1)
							break ; 
						//if succesfull, create new client object for each connection 
						Client	newClient(clientSocket);
						//save address in object
						newClient.setIP(&client_addr);
						//insert new client into client map
						std::pair<ft::ClientMap::iterator, bool> insert_return = client_map.insert(make_pair(newClient.getKey(), newClient));
						//if there is already a client with the same ip adress + socket, insertion fails
						if (insert_return.second == false)
						{
							std::cout << "Connection request failed: duplicate key." << std::endl;
							close(newClient.getSocket());
						}
						else
							std::cout << "Client succesfully connected from " << newClient.getIP() << " on socket " << newClient.getSocket() << ". Total number of connected clients: " << client_map.size() << std::endl;
					}
				}
				else
				{
					while (true)
					{
						char	buffer[80]; //TBD: max size of messages
						int recv_return = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);//TBD if we need any flag for recv?
						if (recv_return < 0)//TBD ob exit oder lieber nur break ;
							break;
						else if (recv_return == 0)//connection has been closed - OPEN: close the fd, remove the client object
							break;
						else
							std::cout << "Message received: " << buffer << std::endl; 
					}
				}
			}
		}

		
		// //clear the buffer and set the clientSocket to non-blocking
		// bzero(buffer, sizeof(buffer));
		// int n = fcntl(clientSocket, F_SETFL, O_NONBLOCK);
		// if (n == -1) {
		// 	std::cerr << "ERROR on fcntl" << std::endl;
		// 	exit(1);
		// }

		// // receive data from a client
		// bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
		// if (bytes_received == -1) {
		// 	std::cerr << "ERROR reading from socket" << std::endl;
		// 	close(sockfd);
		// 	exit(1);
		// }

		// // forward data to other clients
		// bytes_sent = send(clientSocket, buffer, sizeof(buffer), 0);
		// if (bytes_sent == -1)
		// 	exit(1);

		// getsockname(clientSocket, (struct sockaddr *)&client_addr, &clientSize);
		// std::cout << "Here is the message: " << buffer << " :from client " 
		// 		<< inet_ntoa(client_addr.sin_addr) << std::endl;

	}

	// close(clientSocket); //moved to Client destructor
	close(sockfd);

	return 0;
}

