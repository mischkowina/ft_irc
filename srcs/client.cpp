#include "../include/client.hpp"

//Constructor with socket descriptor as parameter
Client::Client(int socket) : socket(socket) {}

Client::Client() {}

void	Client::setIP(sockaddr_in *client_addr)
{
	this->ip_addr = inet_ntoa((struct in_addr)client_addr->sin_addr);
}

std::string	Client::getIP() const
{
	return this->ip_addr;
}
