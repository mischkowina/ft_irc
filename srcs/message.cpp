#include "../include/message.hpp"

Message::Message(std::string msg)
{
	//checks if the message is within maximum length according to IRC documentation
	if (msg.length() > 510)
	{
		this->_isValid = false;
		//send error message to client(would need client object as parameter)?? or in calling function??
		return ;
	}

	//checks for a prefix and extracts it, then erases it from message string
	if (msg[0] == ':')
	{
		this->_prefix = msg.substr(0, msg.find_first_of(' ', 0));
		msg.erase(0, msg.find_first_of(' ', 0));
	}

	//skip & erase all spaces
	size_t	i = 0;
	while (msg[i] == ' ' && i < msg.length())
		i++;
	if (i > 0)
		msg.erase(0, i);

	//extracts everything before the next space as the command
	if (!msg.empty())
	{
		this->_command = msg.substr(0, msg.find_first_of(' ', 0));
		msg.erase(0, msg.find_first_of(' ', 0) + 1);
	}
	//if the string is empty after the prefix or the command string is empty 
	else
	{
		this->_isValid = false;
		//send error message to client(would need client object as parameter)?? or in calling function??
		return ;
	}

	//parse parameters
	
}

Message::~Message() {}

bool	Message::isValid() const
{
	return this->_isValid;
}