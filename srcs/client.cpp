#include "client.hpp"

//Constructor with socket descriptor as parameter
Client::Client(int socket, bool hasPass) : _socket(socket), _username(""), _hasPass(hasPass)
{
	// set new socket to non blocking
	int fcntl_return = fcntl(this->_socket, F_SETFL, O_NONBLOCK);
	if (fcntl_return == -1)
	{
		std::cerr << "ERROR on fcntl" << std::endl;
		close(this->_socket);
		exit(1);
	}

	this->_recvBuffer = "";
	this->_isAuthorized = false;
	this->_isOperator = false;
}

Client::Client(Client const &rhs)
{
	if (this != &rhs)
		*this = rhs;
}

Client::~Client() {}

void	Client::setNick(std::string nick)
{
	this->_nick = nick;
}

void	Client::setUserData(std::vector<std::string> &parameters)
{
	this->_username = parameters[0];
	this->_hostname = parameters[0];
	this->_servername = parameters[0];
	this->_realname = parameters[0];
}

void	Client::setIsAuthorized(bool status)
{
	this->_isAuthorized = status;
}

void	Client::setHasPass(bool status)
{
	this->_hasPass = status;
}

Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->_IP = rhs._IP;
		this->_nick = rhs._nick;
		this->_username = rhs._username;
		this->_hostname = rhs._hostname;
		this->_servername = rhs._servername;
		this->_realname = rhs._realname;
		this->_hasPass = rhs._hasPass;
		this->_isAuthorized = rhs._isAuthorized;
		this->_isOperator = rhs._isOperator;
		this->_recvBuffer = rhs._recvBuffer;
		//TBD
	}
	return (*this);
}

void	Client::setIP(sockaddr_in *client_addr)
{
	this->_IP = inet_ntoa((struct in_addr)client_addr->sin_addr);
	this->_nick = this->_IP + ":";
	this->_nick.append(ft::itos(this->_socket));
}

void	Client::setIsOperator(bool status)
{
	this->_isOperator = status;
}

void	Client::addToRecvBuffer(char *buffer, int len)
{
	std::string	msg(buffer, len);
	this->_recvBuffer += msg;
}

int	Client::getSocket() const
{
	return this->_socket;
}

std::string	Client::getIP() const
{
	return this->_IP;
}

std::string	Client::getNick() const
{
	return this->_nick;
}

std::string	Client::getName() const
{
	return _username;
}

bool	Client::getHasPass() const
{
	return this->_hasPass;
}

bool	Client::getIsAuthorized() const
{
	return this->_isAuthorized;
}

bool	Client::getIsOperator() const
{
	return this->_isOperator;
}

std::string	Client::getRecvBuffer() const
{
	return this->_recvBuffer;
}

void	Client::clearRecvBuffer(int end)
{
	this->_recvBuffer.erase(0, end);
	if (this->_recvBuffer[0] == '\r')
		this->_recvBuffer.erase(0, 1);
	if (this->_recvBuffer[0] == '\n')
		this->_recvBuffer.erase(0, 1);
}

void	Client::sendErrMsg(Server *server, std::string const err_code, char const *err_param)
{
	std::string	err_msg = err_code;

	if (err_msg.find("<") && err_param != NULL && err_msg.find(">"))
	{
		int pos1 = err_msg.find("<");
		int pos2 = err_msg.find(">");
		err_msg.erase(pos1, pos2 - pos1 + 1);
		err_msg.insert(pos1, err_param);
	}
	err_msg.insert(4, _nick + " ");
	err_msg.insert(0, ":" + server->getHostname() + " ");
	std::cout << RED "Sending to " << this->_nick << ": " RESET << err_msg << std::endl;
	err_msg.append("\r\n");
	send(this->_socket, err_msg.data(), err_msg.length(), 0);
}

void	Client::sendMsg(Client &sender, std::string msg, std::string type) const
{
	if (msg.find(" ", 0) != std::string::npos)
		msg.insert(0, ":");
	msg.insert(0, " " + this->_nick + " ");
	//insert command name
	msg.insert(0, type);

	msg.insert(0, " ");
	//insert full prefix of sender - :<nick>!<username>@<IP>
	msg.insert(0, sender.getIP());
	msg.insert(0, "@");
	msg.insert(0, sender.getName());
	msg.insert(0, "!");
	msg.insert(0, sender.getNick());
	msg.insert(0, ":");
	std::cout << BLUE "Sending to " << this->_nick << ": " RESET << msg << std::endl;
	msg.append("\r\n");
	send(this->_socket, msg.data(), msg.size(), 0);
}
