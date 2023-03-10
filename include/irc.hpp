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

# include "client.hpp"
# include "message.hpp"

namespace ft
{
	typedef std::map<std::string, class Client>	ClientMap;

	std::string	itos(int i);
	std::string	skipSpaces(std::string msg);
}

#endif