
#include "server.hpp"

Server::Server(int port, std::string pass) : _portNum(port), _password(pass), _noAuthorization(false), _clientMapChanged(false)
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
	cmd["CONNECT"] = &connect;
	cmd["JOIN"] = &join;
	cmd["HELP"] = &help;
	cmd["CLOSE"] = &closeChannel;
	cmd["PART"] = &closeChannel;
	cmd["INFO"] = &info;
	cmd["WHOIS"] = &whois;
	cmd["NICK"] = &nick;
	cmd["PRIVMSG"] = &privmsg;
	cmd["NAMES"] = &displayNames;
	cmd["PASS"] = &pass_cmd;

	_cmdMap = cmd;
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

std::string	Server::getHostname() const
{
	return this->_hostname;
}

ft::ClientMap	Server::getClientMap() const
{
	return _clients;
}

ft::ChannelMap	Server::getChannelMap() const
{
	return _channels;
}

void	Server::eraseFromClientMap(Client &client)
{
	_clients.erase(client.getNick());
	this->_clientMapChanged = true;
}

bool	Server::addClient(Client &client)
{
	ft::ClientMap::const_iterator it = _clients.find(client.getNick());
	if (it == _clients.end())
	{
		_clients.insert(std::make_pair(client.getNick(), client));
		return true;
	}
	return false;	
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
			std::cout << "Connection with " << it->second.getIP() << " on socket " << it->second.getSocket() << " lost." << std::endl;
			close(pollfds[i].fd);
			_clients.erase(it);
			break;				// from continue -> break; return to the top
		}
		//check for POLLIN events
		if (pollfds[i].revents & POLLIN)
		{
			char	buffer[1024]; //TBD: max size of messages
			Client	currentClient = (*it).second;
			
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
				std::cout << "Connection with " << currentClient.getIP() << " on socket " << currentClient.getSocket() << " lost." << std::endl;
				close(pollfds[i].fd);
				_clients.erase(it);
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
				std::cout << "Full message received from " << currentClient.getNick() << " :" << std::endl << msg << std::endl;
				//process the message (further parse it and execute the according command)
				this->process_request(currentClient, msg);
				if (this->_clientMapChanged == true)
					break ;
			}
		}
		if (this->_clientMapChanged == true)
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
			std::pair<ft::ClientMap::iterator, bool> insert_return = _clients.insert(make_pair(newClient.getNick(), newClient));
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

	//TESTING:				/help  /info  /join #newchannel
	std::cout << "Prefix: " << message.getPrefix() << std::endl;		// this should be '/'
	std::cout << "Command: " << message.getCommand() << std::endl;
	std::cout << "Parameters:" << std::endl;
	std::vector<std::string>	parameters = message.getParameters();
	for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
		std::cout << *it << std::endl;


	// if (message.isCommand() == true)
	this->execCmd(client, message);

	//next steps:
	// implement functions, channels, ...
}

// void	execCmd(Message& msg)
void	Server::execCmd(Client &client, Message& msg)
{

	std::map<std::string, FuncPtr>::const_iterator it = _cmdMap.find(msg.getCommand());
	if (it == _cmdMap.end())
	{
		std::cerr << "Error: Invalid command " << msg.getCommand() << std::endl;
		return;
	}
	//check if the User already used PASS, NICK and USER (except QUIT??)
	(*it->second)(this, client, msg);
}
//////////////////////////////////////////////////////////////////////////////////

void findReceivers(Server *server, std::vector<std::string> listOfRecv, std::string msg)
{
	// bool found = false;
	ft::ClientMap tmpClient = server->getClientMap();
	ft::ChannelMap tmpChannel = server->getChannelMap();

	// iterate through server's clients

	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++)
	{
		ft::ClientMap::const_iterator itClien = tmpClient.find(*itRecv);
		if (itClien != tmpClient.end())
		{
			//OPEN: NEEDS FORMATING
			send(itClien->second.getSocket(), msg.data(), msg.size(), 0);
			continue ;
		}
		ft::ChannelMap::const_iterator itChannel = tmpChannel.find(*itRecv);
		if (itChannel != tmpChannel.end())
		{
			std::list<Client> tmpChannelUsers = itChannel->second.getChannelUsers();
			for (std::list<Client>::const_iterator itChannelRecv = tmpChannelUsers.begin(); itChannelRecv != tmpChannelUsers.end(); itChannelRecv++)
			{
				//OPEN: NEEDS FORMATING
				send(itChannelRecv->getSocket(), msg.data(), msg.size(), 0);
			}
			continue ;
		}
		//ERROR: receiver not found
	}

	
	// for (ft::ClientMap::const_iterator itClien = tmpClient.begin(); itClien != tmpClient.end(); ++itClien)
	// {
	// 	std::cout << "Clients TEST: " <<itClien->second.getNick() << std::endl;
	// 	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++) {
	// 		if (itClien->second.getNick() == *itRecv)
	// 		{
	// 			send(itClien->second.getSocket(), msg.data(), msg.size(), 0);
	// 			found = true;
	// 		}
	// 	}
	// }

	// ft::ChannelMap tmpChannel = server->getChannelMap();
	// Channel 	dummyChannel("dummy");
	// tmpChannel.insert(std::make_pair("dummy", dummyChannel));
	// // iterate through channels' clients
	// for (ft::ChannelMap::const_iterator itChannel = tmpChannel.begin(); itChannel != tmpChannel.end(); ++itChannel)
	// {
	// 	std::cout << "Channel TEST: " <<itChannel->second.getChannelName() << std::endl;
	// 	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++) {
	// 		if (itChannel->second.getChannelName() == *itRecv)
	// 		{
	// 			// iterate through channel's list of users
	// 			std::list<Client> tmpChannelUsers = itChannel->second.getChannelUsers();
	// 			for (std::list<Client>::const_iterator itChannelRecv = tmpChannelUsers.begin(); itChannelRecv != tmpChannelUsers.end(); itChannelRecv++) {
	// 				send(itChannelRecv->getSocket(), msg.data(), msg.size(), 0);
	// 			}
	// 			found = true;
	// 		}
	// 	}
	// }

	// if (found == false)
	// {
	// 	// return msg to client ? check irc docs
	// 	throw std::runtime_error("no receivers found");
	// }
}

// send (private) msg to a specific user/channel
void	privmsg(Server *server, Client &client, Message& msg)
{
	// (void)server;
	(void)client;
	// (void)msg;

	//parse first parameter by commas to get all the recipients -> <receiver> can be a list of names or channels
	// Parameters: <receiver>{,<receiver>} <text to be sent>

	// parse parameters inside of each function seperately 
	std::vector<std::string>	parameters = msg.getParameters();
	std::vector<std::string>	receivers;
	std::string textToBeSend = parameters[1];

	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		receivers.push_back(token);
		token.clear();
	}

	// std::cout << "TEST1: " << textToBeSend << std::endl;
	// std::cout << "TEST2: " << std::endl;
	// for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	// 	std::cout << *it << std::endl;

	try {
		findReceivers(server, receivers, textToBeSend);
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;	// adjust appropriate response here
	}

	//send the message (2nd parameter) to all receivers
	// --> get the socket descriptor of each receiver and send the message
	// client->_name: 2nd parameter
	// e.g.: Primoz: Hi 

}

void	displayNames(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display all users of a specific channel(s)
		// go through the channel's list of users
}

// ahhh .... check the documentation
void	connect(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// establish initial connection -> server should add this new client
	// and enable him to open/manage new channels and usage of general functions
}

void	join(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	//create new channel [#name] if it doesn't exist yet
	std::cout << " ----> i want to get into a channel!\n";
}

void	help(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display standard stuff ...
	std::cout << " ----> what help?\n";
}

void	closeChannel(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// leave channel, and if it's empty, remove it
}

void	info(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display standard stuff ...

}

void	whois(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display info about specific user
		// go through the channel's list of users
		// and return error / user's info
}

void	nick(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();
	
	//check if there is at least one parameter
	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NONICKNAMEGIVEN, NULL);
		return ;
	}
	
	//check if first parameter is a valid nickname
	if (ft::isValidNick(parameters[0]) == false)
	{
		client.sendErrMsg(server, ERR_ERRONEUSNICKNAME, parameters[0].c_str());
		return ;
	}

	// try changing the nickname of the client
	// original has to be erased and a copy with the new nick to be inserted in order for map to work properly (no changing of keys)
	Client	changedClient(client);
	changedClient.setNick(parameters[0]);

	if (server->addClient(changedClient) == false)
		client.sendErrMsg(server, ERR_NICKNAMEINUSE, parameters[0].c_str());
	else
		server->eraseFromClientMap(client);
	//COMMENT: No implementation of ERR_NICKCOLLISION since it is only applicable for multi-server connections
}

void	pass_cmd(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();

	//check if client is already authorized (already used PASS before)
	std::cout << "Client is Authorized:" << client.getIsAuthorized() << std::endl;
	if (client.getIsAuthorized() == true)
	{
		client.sendErrMsg(server, ERR_ALREADYREGISTRED, NULL);
		return ;
	}
	
	//check if there is a password parameter
	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
		return ;
	}

	//check if the password is correct, if so change status of client to isAuthorized == true
	if (parameters[0] == server->getPass())
		client.setIsAuthorized(true);
		
}
