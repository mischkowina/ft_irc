#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <string>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <vector>
# include <list>
# include <map>
# include <algorithm>
# include <utility>
# include <iterator>
# include <sstream>

# include <fcntl.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/in.h>

# include <signal.h>
# include <poll.h>
# include <sys/select.h>

# include "channel.hpp"
# include "client.hpp"
# include "message.hpp"
# include "error.hpp"
# include "reply.hpp"
# include "cmds.hpp"
# include "server.hpp"

#define RED     "\033[1m\033[31m"      /* Bold Red */
#define BLUE "\033[1m\033[36m"      /* Bold Cyan */
#define YELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define RESET "\033[0m"

namespace ft
{
	std::string	itos(int i);
	std::string	skipCharacter(std::string msg, char c);
	bool		isValidNick(std::string &nick);
	bool		isSpecial(char c);
}

#endif
