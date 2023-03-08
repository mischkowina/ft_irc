#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irc.hpp"

class Client {
	private:
		int			_socket;
		std::string	_IP;
		std::string	_key;// = IP:socket; e.g. 127.0.0.1:4
		std::string	_nick;
		std::string	_name;
		bool		_isOperator;

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
