#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../include/irc.hpp"

class Client {
	private:
		int			socket;
		std::string	address;//IP adress + socket (!= port) -> 0.0.0.0:80
		std::string	nick;
		std::string	name;
		bool		isOperator;

	public:
		Client(int socket);
		~Client();

		void	setAddress(sockaddr_in *client_addr);
		int		getSocket() const;
		std::string	getAddress() const;
};

#endif
