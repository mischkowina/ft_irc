#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../include/irc.hpp"

class Client {
	private:
		int const	socket;
		std::string	ip_addr;

	public:
		Client(int socket);
		~Client();

		void	setIP(sockaddr_in *client_addr);
		std::string	getIP() const;
};

#endif
