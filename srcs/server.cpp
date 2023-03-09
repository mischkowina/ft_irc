
#include "server.hpp"

Server::Server(int port, std::string pass) : _portNum(port), _password(pass)
{
	// create a socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// TODO: ALL neTWORK INTERFACES
	if (_sockfd == -1) {
		std::cerr << "ERROR opening socket" << std::endl;
		exit(1);
	}
	// set socket descriptor to be reuseable
	int on = 1;
	if (setsockopt(_sockfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) == -1) {
		std::cerr << "ERROR setsockopt()" << std::endl;
		close(_sockfd);
		exit(1);
	}
	// set socket descriptor to be nonblocking
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "ERROR fcntl" << std::endl;
		exit(1);
	}
	// bind the socket to an IP / port
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_portNum);		// small endian -> big endian
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_sockfd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1) {
		std::cerr << "ERROR binding" << std::endl;
		close(_sockfd);
		exit(1);
	}
	// set the socket for listening to port
	if (listen(_sockfd, SOMAXCONN) == -1) {
		std::cerr << "ERROR listening" << std::endl;
		close(_sockfd);
		exit(1);
	}
	std::cout << "Server running." << std::endl;
}

Server::~Server(void)
{
	close(_sockfd);
}

std::string	Server::getPass() const
{
	return _password;
}

int		Server::getServerSoc() const
{
	return _sockfd;
}

void	Server::run()
{
	//create struct pollfd[] with the appropriate size (number of connected clients + 1 for the listening socket)
	// struct pollfd pollfds[_clients.size() + 1];
	std::vector<pollfd> pollfds(1);

	//initalize the first struct pollfd[0] to the listening socket
	bzero(&pollfds[0], sizeof(pollfds[0]));
	pollfds[0].fd = _sockfd;
	pollfds[0].events = POLLIN;

	while (true)
	{
		int i = 1;

		//populate the rest of the struct pollfd[] with the client socket descriptors - doesn't happen until a connection is established
		pollfds.resize(_clients.size() + 1);
		for (ft::ClientMap::iterator it = _clients.begin(); it != _clients.end(); it++, i++)
		{
			bzero(&pollfds[i], sizeof(pollfds[i]));
			pollfds[i].fd = it->second.getSocket();
			pollfds[i].events = POLLIN;
		}

		//poll
		int pollreturn = poll(pollfds.data(), _clients.size() + 1, 0);
		if (pollreturn < 0)
		{
			std::cerr << "ERROR on poll" << std::endl;
			exit(1);//tbd if we want to exit or just continue running the server??
		}
		else if (pollreturn == 0)
			continue ;

		//check all client sockets (of the clients) for events
		this->checkAllClientSockets(pollfds);

		//check for POLLIN on listening socket -> pending connections
		this->checkListeningSocket(pollfds);
	}
}

void	Server::checkAllClientSockets(std::vector<pollfd> pollfds)
{
	int	i = 1;
	for (ft::ClientMap::iterator it = _clients.begin(); it != _clients.end(); it++, i++)
	{
		//skip all sockets without an event
		if (pollfds[i].revents == 0)
			continue ;
		//check socket for error events, if yes close the connection and delete the client from the map
		if (pollfds[i].revents & POLLERR || pollfds[i].revents & POLLHUP)
		{
			std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost because POLLERR/POLLHUP." << std::endl;
			close(pollfds[i].fd);
			_clients.erase(it);
			break;				// from continue -> break; return to the top
		}
		//check for POLLIN events
		if (pollfds[i].revents & POLLIN)
		{
			char	buffer[1024]; //TBD: max size of messages
			Client	currentClient = it->second;
			
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
				std::cout << "Connection with " << currentClient.getIP() << " on socket " << currentClient.getSocket() << " lost because recv." << std::endl;
				close(pollfds[i].fd);
				_clients.erase(it);
				break ; // from continue -> break; return to the top, same as line 105
			}
			//echo that message back to the client who send it --> REMOVE LATER
			int send_return = send(pollfds[i].fd, buffer, recv_return, 0);
			if (send_return < 0)
			{
				std::cerr << "ERROR on send" << std::endl;
				std::cout << "Closing connection with " << currentClient.getIP() << " on socket " << currentClient.getSocket() << " ." << std::endl;
				close(pollfds[i].fd);
				_clients.erase(it);
			}
			currentClient.addToRecvBuffer(buffer, recv_return);
			std::string	msg = currentClient.getRecvBuffer();
			//check if the message was correctly terminated by \r\n, if not keep it in the Clients recvBuffer
			size_t msg_end = msg.find_last_of("\r\n");
			if (msg_end == std::string::npos)
			{
				std::cout << "Incomplete message from " << currentClient.getKey() << " - storing for later." << std::endl;
				continue ;
			}

			//get the full message to work with
			msg = currentClient.getRecvBuffer().substr(0, msg_end);
			//clear the message from the buffer (potentially keeping content that follows \r\n)
			currentClient.clearRecvBuffer(msg_end);
			std::cout << "Message received: " << msg << std::endl;
		}
	}
}

void	Server::checkListeningSocket(std::vector<pollfd> pollfds)
{
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
			clientSocket = accept(_sockfd, (sockaddr *)&client_addr,	&clientSize); 
			if (clientSocket == -1)
				break ; 
			//if succesfull, create new client object for the connection 
			Client	newClient(clientSocket);
			//save address in object
			newClient.setIP(&client_addr);
			//insert new client into client map
			std::pair<ft::ClientMap::iterator, bool> insert_return = _clients.insert(make_pair(newClient.getKey(), newClient));
			//if there is already a client with the same ip adress + socket, insertion fails
			if (insert_return.second == false)
			{
				std::cout << "Connection request failed: duplicate key." << std::endl;
				close(newClient.getSocket());
			}
			else
				std::cout << "Client succesfully connected from " << newClient.getIP() << " on socket " << newClient.getSocket() << ". Total number of connected clients: " << _clients.size() << std::endl;
		}
	}
}