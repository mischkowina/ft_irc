#include "client.hpp"

//Constructor with socket descriptor as parameter
Client::Client(int socket, bool isAuthorized) : _socket(socket), _isAuthorized(isAuthorized)
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

void	Client::setIsAuthorized(bool status)
{
	this->_isAuthorized = status;
}

Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->_IP = rhs._IP;
		this->_nick = rhs._nick;
		this->_name = rhs._name;
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
	return _name;
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

	if (err_msg.find("<") == 4 && err_param != NULL && err_msg.find(">"))
	{
		int pos = err_msg.find(">");
		err_msg.erase(4, pos - 3);
		err_msg.insert(4, err_param);
	}
	err_msg.insert(4, _nick + " ");
	err_msg.insert(0, ":" + server->getHostname() + " ");
	err_msg.append("\r\n");
	send(this->_socket, err_msg.data(), err_msg.length(), 0);
}
