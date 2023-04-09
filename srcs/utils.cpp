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

//according to 2.3.1 in RFC2812 (Update)
bool	ft::isValidNick(std::string &nick)
{
	if (nick.length() > 9)
		return false;
	if (isalpha(nick.at(0)) == 0 && ft::isSpecial(nick.at(0)) == false)
		return false;
	for (size_t i = 1; i < nick.length(); i++)
	{
		if (isalnum(nick[i]) == 0 && ft::isSpecial(nick[i]) == false)
			return false;
	}
	return true;
}

bool	ft::isSpecial(char c)
{
	if (c < 91 || c > 125)
		return false;
	if (c > 96 && c < 123)
		return false;
	return true;
}
