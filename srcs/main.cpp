#include "irc.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		std::cerr << "Correct use: ./ircserv <port> <password>" << std::endl;

	try {
		Server IRCserver(atoi(argv[1]), std::string (argv[2]));
		IRCserver.run();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	std::cout << "Server shutting down." << std::endl;
	return 0;
}
