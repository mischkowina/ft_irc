#include "../include/irc.hpp"

std::string	ft::itos(int i)
{
	std::stringstream	stream;
	std::string			str;
	stream << i;
	stream >> str;
	return (str);
}