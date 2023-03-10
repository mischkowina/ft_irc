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

# include "channel.hpp"
# include "client.hpp"
# include "message.hpp"

class Server;

namespace ft
{
	typedef std::map<std::string, class Client>		ClientMap;
	typedef std::map<std::string, class Channel>	ChannelMap;

	std::string	itos(int i);
	std::string	skipCharacter(std::string msg, char c);
}

#endif

/*

This error is likely caused by the fact that you are using a
 member function pointer instead of a regular function pointer. 
 Member function pointers are different from regular function 
 pointers because they have an implicit this pointer that needs 
 to be passed in when calling the function.

To fix the error, you can change the definition of FuncPtr to 
use a regular function pointer instead of a member function 
pointer, like this:

using FuncPtr = void (*)(Server&, Message&);

*/