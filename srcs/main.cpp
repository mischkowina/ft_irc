#include "irc.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	// if (argc != 3)
	// 	std::cerr << "Correct use: ./ircserv <port> <password>" << std::endl;

	// Server IRCserver(atoi(argv[1]), std::string (argv[2]));
	Server		IRCserver(6667, "P");
	IRCserver.run();
	std::cout << "Server shutting down." << std::endl;

	return 0;
}
