
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

	//map container to store all the client objects
	ft::ClientMap	client_map;//typedef in irc.hpp
	
	while (true)
	{
		//create struct pollfd[] with the appropriate size (number of connected clients + 1 for the listening socket)
		struct pollfd pollfds[client_map.size() + 1];

		//initalize the first struct pollfd[0] to the listening socket
		bzero(&pollfds[0], sizeof(pollfds[0]));
		pollfds[0].fd = sockfd;
		pollfds[0].events = POLLIN;

		//populate the rest of the struct pollfd[] with the client socket descriptors - doesn't happen until a connection is established
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
		
		// check for error on listening socket
		if (pollfds[0].revents & POLLERR || pollfds[0].revents & POLLHUP)
		{
			std::cout << "Problem on listening socket?" << std::endl; //TBD if necessary
		}
		//check the listening socket for new pending connections
		if (pollfds[0].revents & POLLIN)
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
				//if succesfull, create new client object for the connection 
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

		//check all other sockets (of the clients) for events
		i = 1;
		for (ft::ClientMap::iterator it = client_map.begin(); it != client_map.end(); it++, i++)
		{
			//skip all sockets without an event
			if (pollfds[i].revents == 0)
				continue ;
			//check socket for error events, if yes close the connection and delete the client from the map
			if (pollfds[i].revents & POLLERR || pollfds[i].revents & POLLHUP)
			{
				std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost." << std::endl;
				close(pollfds[i].fd);
				client_map.erase(it);
				continue;
			}
			//check for POLLIN events
			if (pollfds[i].revents & POLLIN)
			{
				char	buffer[1024]; //TBD: max size of messages
				
				//receive message from socket
				int recv_return = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
				if (recv_return < 0)
				{
					std::cerr << "ERROR on recv" << std::endl;
					exit(1);
				}
				//if recv returns 0, the connection has been closed/lost on the client side -> close connection and delete client
				else if (recv_return == 0)
				{
					std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost." << std::endl;
					close(pollfds[i].fd);
					client_map.erase(it);
					continue; 
				}
				else
				{
					//Display received message in terminal
					std::cout << "Message received: " << buffer << std::endl;
					//echo that message back to the client who send it
					int send_return = send(pollfds[i].fd, buffer, recv_return, 0);
					if (send_return < 0)
					{
						std::cerr << "ERROR on send" << std::endl;
						std::cout << "Closing connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " ." << std::endl;
						close(pollfds[i].fd);
						client_map.erase(it);
					}
				}
			}
		}
	}
	//TO-DO: close all client sockets
	close(sockfd);

	return 0;
}

