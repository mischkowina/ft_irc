#include "../include/client.hpp"

//Constructor with socket descriptor as parameter
Client::Client(int socket) : _socket(socket)
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

Client::~Client() {}

void	Client::setIP(sockaddr_in *client_addr)
{
	this->_IP = inet_ntoa((struct in_addr)client_addr->sin_addr);
	this->_key = this->_IP + ":";
	this->_key.append(ft::itos(this->_socket));
}

Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->_IP = rhs._IP;
		this->_key = rhs._key;
		this->_nick = rhs._nick;
		this->_name = rhs._name;
		this->_isOperator = rhs._isOperator;
		this->_recvBuffer = rhs._recvBuffer;
	}
	return (*this);
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

std::string	Client::getKey() const
{
	return this->_key;
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
	this->_recvBuffer.erase(0, end + 1);
}
