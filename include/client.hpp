#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irc.hpp"

class Server;

class Client {
	private:
		int			_socket;
		std::string	_IP;
		std::string	_nick;
		std::string	_name;
		bool		_isAuthorized;
		bool		_isOperator;
		std::string	_recvBuffer;

	public:
		Client(int socket, bool isAuthorized);
		Client(Client const &rhs);
		~Client();
		Client	&operator=(Client const &rhs);

		void		setIP(sockaddr_in *client_addr);
		void		setNick(std::string nick);
		void		setIsAuthorized(bool status);
		void		setIsOperator(bool status);
		void		addToRecvBuffer(char *buffer, int len);
		int			getSocket() const;
		std::string	getIP() const;
		std::string	getNick() const;
		std::string	getName() const;
		bool		getIsAuthorized() const;
		bool		getIsOperator() const;
		std::string	getRecvBuffer() const;

		void		clearRecvBuffer(int end);
		void		sendErrMsg(Server *server, std::string const err_code, char const *err_param);

		


};

#endif
