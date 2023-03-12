#include "irc.hpp"

std::string	ft::itos(int i)
{
	std::stringstream	stream;
	std::string			str;
	stream << i;
	stream >> str;
	return (str);
}

std::string	ft::skipSpaces(std::string msg)
{
	size_t	i = 0;
	while (msg[i] == ' ' && i < msg.length())
		i++;
	if (i > 0)
		msg.erase(0, i);
	return msg;
}
