#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irc.hpp"

class Server;

class Client {
	private:
		int			_socket;
		std::string	_IP;
		std::string	_nick;
		std::string	_username;
		std::string	_hostname;
		std::string	_servername;
		std::string	_realname;
		bool		_hasPass;
		bool		_isAuthorized;
		bool		_isOperator;
		int 		_channelCounter;
		bool		_isInvisible;
		std::string	_awayMsg;
		std::string	_recvBuffer;

	public:
		Client(int socket, bool isAuthorized);
		Client(Client const &rhs);
		~Client();
		Client	&operator=(Client const &rhs);

		void		setIP(sockaddr_in *client_addr);
		void		setNick(std::string nick);
		void		setUserData(std::vector<std::string> &parameters);
		void		setIsAuthorized(bool status);
		void		setHasPass(bool status);
		void		setIsOperator(bool status);
		bool		setIsInvisible(bool status);
		void		setAwayMsg(std::string msg);
		void		addToRecvBuffer(char *buffer, int len);
	
		int			getSocket() const;
		std::string	getIP() const;
		std::string	getNick() const;
		std::string	getName() const;
		bool		getIsAuthorized() const;
		bool		getHasPass() const;
		bool		getIsOperator() const;
		bool		isInvisible() const;
		std::string	getAwayMsg() const;
		std::string	getRecvBuffer() const;

		bool		maxNumOfChannels();
		void		clearRecvBuffer(int end);
		void		decreaseChannelCounter();
		void		increaseChannelCounter();

		void		sendMsg(Client &sender, std::string msg, std::string type) const;
		void		sendErrMsg(Server *server, std::string const err_code, char const *err_param);
		void		sendErrMsg(Server *server, std::string const err_code, std::vector<std::string> err_param);
};

#endif
