#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../include/irc.hpp"

class Client {
	private:
		int			socket;
		std::string	IP;
		std::string	key;// = IP:socket; e.g. 127.0.0.1:4
		std::string	nick;
		std::string	name;
		bool		isOperator;

	public:
		Client(int socket);
		~Client();

		void		setIP(sockaddr_in *client_addr);
		void		setIsOperator(bool status);
		int			getSocket() const;
		std::string	getIP() const;
		std::string	getKey() const;
		bool		getIsOperator() const;
};

#endif
