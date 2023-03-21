#include "irc.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	// if (argc != 3)
	// 		std::cerr << Correct use: ./ircserv <port> <password> << std::endl;

	// bool endServer = false;
	// IRCserver(atoi(argv[1]), std::string str(argv[2]);
	std::string	pass=  "password";
	Server		IRCserver(6667, pass);

	IRCserver.run();

	//TO-DO: close all client sockets

	return 0;
}
