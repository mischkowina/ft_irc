#include "irc.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	// if (argc != 3)
	// 		std::cerr << Correct use: ./ircserv <port> <password> << std::endl;

	// bool endServer = false;
	// IRCserver(atoi(argv[1]), std::string str(argv[2]);
	std::string	pass=  "password";
	Server		IRCserver(6667,pass);

	//map container to store all the client objects
	ft::ClientMap	client_map;			//typedef in irc.hpp

	//create struct pollfd[] with the appropriate size (number of connected clients + 1 for the listening socket)
	// struct pollfd pollfds[client_map.size() + 1];
	std::vector<pollfd> pollfds(1);

	//initalize the first struct pollfd[0] to the listening socket
	bzero(&pollfds[0], sizeof(pollfds[0]));
	pollfds[0].fd = IRCserver.getServerSoc();
	pollfds[0].events = POLLIN;

	while (true)
	{
		int i = 1;

		//populate the rest of the struct pollfd[] with the client socket descriptors - doesn't happen until a connection is established
		pollfds.resize(client_map.size() + 1);
		for (ft::ClientMap::iterator it = client_map.begin(); it != client_map.end(); it++, i++)
		{
			bzero(&pollfds[i], sizeof(pollfds[i]));
			pollfds[i].fd = it->second.getSocket();
			pollfds[i].events = POLLIN;
		}

		//poll
		int pollreturn = poll(pollfds.data(), client_map.size() + 1, 0);
		if (pollreturn < 0)
		{
			std::cerr << "ERROR on poll" << std::endl;
			exit(1);//tbd if we want to exit or just continue running the server??
		}
		else if (pollreturn == 0)
			continue ;

		//check all client sockets (of the clients) for events
		i = 1;
		for (ft::ClientMap::iterator it = client_map.begin(); it != client_map.end(); it++, i++)
		{
			//skip all sockets without an event
			if (pollfds[i].revents == 0)
				continue ;
			//check socket for error events, if yes close the connection and delete the client from the map
			if (pollfds[i].revents & POLLERR || pollfds[i].revents & POLLHUP)
			{
				std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost because POLLERR/POLLHUP." << std::endl;
				close(pollfds[i].fd);
				client_map.erase(it);
				break;				// from continue -> break; return to the top
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
					std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost because recv." << std::endl;
					close(pollfds[i].fd);
					client_map.erase(it);
					break ; // from continue -> break; return to the top, same as line 105
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

		// check for error on listening socket
		if (pollfds[0].revents & POLLERR || pollfds[0].revents & POLLHUP)
		{
			std::cout << "Problem on listening socket?" << std::endl; //TBD if necessary
		}
		//check the listening socket for new pending connections
		else if (pollfds[0].revents & POLLIN)
		{
			sockaddr_in	client_addr;
			socklen_t	clientSize = sizeof(client_addr);
			int 		clientSocket;

			//accept all new pending connections
			while (true)
			{
				clientSocket = accept(IRCserver.getServerSoc(), (sockaddr *)&client_addr,	&clientSize); 
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
	}
	//TO-DO: close all client sockets

	return 0;
}
