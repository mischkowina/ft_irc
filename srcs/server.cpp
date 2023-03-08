
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
