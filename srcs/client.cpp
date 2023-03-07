#include "../include/client.hpp"

//Constructor with socket descriptor as parameter
Client::Client(int socket) : socket(socket) {}

Client::~Client()
{
	close(socket);
}

void	Client::setAddress(sockaddr_in *client_addr)
{
	this->address = inet_ntoa((struct in_addr)client_addr->sin_addr);
	this->address += ":";
	this->address.append(ft::itos(this->socket));
}

int	Client::getSocket() const
{
	return this->socket;
}

std::string	Client::getAddress() const
{
	return this->address;
}
