#include "irc.hpp"

std::string	ft::itos(int i)
{
	std::stringstream	stream;
	std::string			str;
	stream << i;
	stream >> str;
	return (str);
}

std::string	ft::skipCharacter(std::string msg, char c)
{
	size_t	i = 0;
	while (msg[i] == c && i < msg.length())
		i++;
	if (i > 0)
		msg.erase(0, i);
	return msg;
}
