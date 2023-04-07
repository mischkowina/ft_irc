#include "server.hpp"

Server::Server(int port, std::string pass) : _portNum(port), _password(pass), _oper_password("operator"), _noAuthorization(false), _clientMapChanged(false), _died(false)
{
	if (this->_password.empty() == true)
		this->_noAuthorization = true;
	
	// create a socket
	// TODO: ALL neTWORK INTERFACES
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1) {
		std::cerr << "ERROR opening socket" << std::endl;
		// TODO: replace with throw()
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

	//get hostname
	char hstnme[256]; // 255 is maxlen for hostname according to manpages
	gethostname(hstnme, 256);
	this->_hostname = std::string(hstnme);

	std::cout << "Server running on " << _hostname << " on port " << _portNum << "." << std::endl;

	// functions
	std::map<std::string, FuncPtr> cmd;
	cmd["NICK"] = &nick;
	cmd["PASS"] = &pass_cmd;
	cmd["USER"] = &user;
	cmd["QUIT"] = &quit;
	cmd["OPER"] = &oper;

	cmd["JOIN"] = &join;
	cmd["PART"] = &part;
	cmd["TOPIC"] = &topic;
	cmd["NAMES"] = &names;
	cmd["LIST"] = &list;
	cmd["INVITE"] = &invite;
	cmd["KICK"] = &kick;

	cmd["MOTD"] = &oper;

	cmd["PRIVMSG"] = &privmsg;
	cmd["NOTICE"] = &notice;

	cmd["PING"] = &ping;
	cmd["PONG"] = &pong;

	cmd["AWAY"] = &away;
	cmd["DIE"] = &die;

	_cmdMap = cmd;
}

Server::~Server(void)
{
	for (ClientMap::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->second.getSocket());

	close(_sockfd);

	std::cout << "Closed all active connections." << std::endl;
}

std::string	Server::getPass() const
{
	return _password;
}

std::string	Server::getOperPass() const
{
	return _oper_password;
}

int		Server::getServerSoc() const
{
	return _sockfd;
}

std::string	Server::getHostname() const
{
	return this->_hostname;
}

sockaddr_in Server::getServerAddr() const
{
	return this->_server_addr;
}

Server::ClientMap	&Server::getClientMap()
{
	return _clients;
}

Server::ClientMap	&Server::getAuthorizedClientMap()
{
	return _authorizedClients;
}

Server::ChannelMap	&Server::getChannelMap()
{
	return _channels;
}

void	Server::setHasDied(bool status)
{
	this->_died = status;
}

void	Server::eraseFromClientMap(Client &client)
{
	_clients.erase(client.getNick());
	_authorizedClients.erase(client.getNick());
	this->_clientMapChanged = true;
}

bool	Server::addClient(Client &client)
{
	Server::ClientMap::const_iterator it = _clients.find(client.getNick());
	if (it == _clients.end())
	{
		_clients.insert(std::make_pair(client.getNick(), client));
		return true;
	}
	return false;	
}

void	Server::addAuthorizedClient(Client &client)
{
	_authorizedClients.insert(std::make_pair(client.getNick(), client));
}

void	Server::run()
{
	//create struct pollfd[] with the appropriate size (number of connected clients + 1 for the listening socket)
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
		for (Server::ClientMap::iterator it = _clients.begin(); it != _clients.end(); it++, i++)
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

		if (this->_died == true)
			break;

		//check for POLLIN on listening socket -> pending connections
		this->checkListeningSocket(pollfds);
	}
}

void	Server::checkAllClientSockets(std::vector<pollfd> pollfds)
{
	int	i = 1;
	for (Server::ClientMap::iterator it = _clients.begin(); it != _clients.end(); it++, i++)
	{
		//skip all sockets without an event
		if (pollfds[i].revents == 0)
			continue ;
		//check socket for error events, if yes close the connection and delete the client from the map
		if (pollfds[i].revents & POLLERR || pollfds[i].revents & POLLHUP)
		{
			Message msg("QUIT :Connection with " + it->second.getNick() + " lost.");
			quit(this, it->second, msg);
			std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost." << std::endl;
			this->_clientMapChanged = false;
			break;
		}
		//check for POLLIN events
		if (pollfds[i].revents & POLLIN)
		{
			char	buffer[1024]; //TBD: max size of messages
			Client	&currentClient = (*it).second;
			
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
				Message msg("QUIT :Connection with " + it->second.getNick() + " lost.");
				quit(this, it->second, msg);
				std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost." << std::endl;
				this->_clientMapChanged = false;
				break ; // from continue -> break; return to the top, same as line 105
			}
			//add message to what is in the clients message buffer
			currentClient.addToRecvBuffer(buffer, recv_return);
			
			//process overall content of the buffer unless it is empty
			while (currentClient.getRecvBuffer().empty() == false)
			{
				std::string	msg = currentClient.getRecvBuffer();
				//find either a \r or \n to signal end of a message
				size_t msg_end = msg.find_first_of("\r\n");
				//if there is none, the message is incomplete and stays stored in the buffer
				if (msg_end == std::string::npos)
				{
					std::cout << "Incomplete message from " << currentClient.getNick() << " - storing for later." << std::endl;
					break ;
				}

				//get the terminated message to work with
				msg = currentClient.getRecvBuffer().substr(0, msg_end);
				//clear the message from the buffer (potentially keeping content that follows \r\n)
				currentClient.clearRecvBuffer(msg_end);
				std::cout << YELLOW << currentClient.getNick() << ": " RESET << msg << std::endl;
				//process the message (further parse it and execute the according command)
				this->process_request(currentClient, msg);
				if (this->_clientMapChanged == true || this->_died == true)
					break ;
					
			}
		}
		if (this->_clientMapChanged == true || this->_died == true)
		{
			this->_clientMapChanged = false;
			break ;
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
			clientSocket = accept(_sockfd, (sockaddr *)&client_addr, &clientSize); 
			if (clientSocket == -1)
				break ;
			//if succesfull, create new client object for the connection 
			Client	newClient(clientSocket, this->_noAuthorization);
			//save address in object
			newClient.setIP(&client_addr);
			//insert new client into client map
			std::pair<Server::ClientMap::iterator, bool> insert_return = _clients.insert(make_pair(newClient.getNick(), newClient));
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

void	Server::process_request(Client &client, std::string msg)
{
	//turn message string into message object --> parsing in Message constructor
	Message	message(msg);
	//check if the message was in valid format -> else return and send an error message to client
	if (message.isValid() == false)
	{
		//send error message to client
		return ; 
	}
	this->execCmd(client, message);
}

// void	execCmd(Message& msg)
void	Server::execCmd(Client &client, Message& msg)
{

	std::map<std::string, FuncPtr>::const_iterator it = _cmdMap.find(msg.getCommand());
	if (it == _cmdMap.end())
	{
		client.sendErrMsg(this, ERR_UNKNOWNCOMMAND, msg.getCommand().c_str());
		return;
	}
	if (msg.getCommand() == "QUIT")//QUIT can always be used no matter the authorization
	{
		quit(this, client, msg);
		return;
	}
	if (client.getHasPass() == false && msg.getCommand() != "PASS")//PASS has to be used before any other command can be used
	{
		client.sendErrMsg(this, ERR_NOTREGISTERED, NULL);
		return ;
	}
	else if (client.getHasPass() == true && client.getIsAuthorized() == false)
	{
		std::string	nick = client.getNick();
		if (msg.getCommand() == "NICK" && ft::isValidNick(nick) == false)
			(*it->second)(this, client, msg);
		else if (msg.getCommand() == "USER" && client.getName().empty() == true)
			(*it->second)(this, client, msg);
		else
			client.sendErrMsg(this, ERR_NOTREGISTERED, NULL);
		return ;
	}
	(*it->second)(this, client, msg);
}

void	Server::createNewChannel(std::string name, Client &client)
{
	Channel tmp(name);
	tmp.setChannelOp(client);
	tmp.setChannelUsers(client);
	_channels.insert(std::make_pair(name, tmp));
}

//////////////////////////////////////////////////////////////////////////////////