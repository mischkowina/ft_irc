#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irc.hpp"

class Client {
	private:
		int			_socket;
		std::string	_IP;
		std::string	_nick;
		std::string	_name;
		bool		_isOperator;
		std::string	_recvBuffer;

	public:
		Client(int socket);
		~Client();
		Client	&operator=(Client const &rhs);

		void		setIP(sockaddr_in *client_addr);
		void		setNick(std::string nick);
		void		setIsOperator(bool status);
		void		addToRecvBuffer(char *buffer, int len);
		int			getSocket() const;
		std::string	getIP() const;
		std::string	getNick() const;
		std::string	getName() const;
		bool		getIsOperator() const;
		std::string	getRecvBuffer() const;

		void		clearRecvBuffer(int end);

		


};

#endif
