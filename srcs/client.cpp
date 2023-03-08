#include "../include/client.hpp"

//Constructor with socket descriptor as parameter
Client::Client(int socket) : socket(socket)
{
	// set new socket to non blocking
	// int fcntl_return = fcntl(this->socket, F_SETFL, O_NONBLOCK);
	// if (fcntl_return == -1)
	// {
	// 	std::cerr << "ERROR on fcntl" << std::endl;
	// 	close(this->socket);
	// 	exit(1);
	// }
}

Client::~Client() {}

void	Client::setIP(sockaddr_in *client_addr)
{
	this->IP = inet_ntoa((struct in_addr)client_addr->sin_addr);
	this->key = this->IP + ":";
	this->key.append(ft::itos(this->socket));
}

void	Client::setIsOperator(bool status)
{
	this->isOperator = status;
}

int	Client::getSocket() const
{
	return this->socket;
}

std::string	Client::getIP() const
{
	return this->IP;
}

std::string	Client::getKey() const
{
	return this->key;
}

bool	Client::getIsOperator() const
{
	return this->isOperator;
}
