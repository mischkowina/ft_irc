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
		if (msg.find(' ', 0) != std::string::npos)
		{
			this->_prefix = msg.substr(1, msg.find(' ', 0));
			msg.erase(0, msg.find(' ', 0));
		}
		//if nothing follows after a prefix, the request is incomplete, error
		else
		{
			this->_isValid = false;
			//send error message to client(would need client object as parameter)?? or in calling function??
			return ;
		}
	}

	//skip & erase all spaces at beginning of the string
	msg = ft::skipSpaces(msg);

	//extracts everything before the next space as the command
	if (!msg.empty())
	{
		//when there is a space after the command, we only take a substring
		if (msg.find(' ', 0) != std::string::npos)
		{
			this->_command = msg.substr(0, msg.find(' ', 0));
			msg.erase(0, msg.find(' ', 0) + 1);
		}
		//if there is no more space after the command, there are no parameters; take over whole string as command
		else
		{
			this->_command = msg;
			this->_isValid = true;
			return ;
		}
	}
	//if there is no command, return & error (every request needs a command)
	else
	{
		this->_isValid = false;
		//send error message to client(would need client object as parameter)?? or in calling function??
		return ;
	}

	//skip & erase all spaces at beginning of the string
	msg = ft::skipSpaces(msg);

	//parse parameters
	while (!msg.empty())
	{
		//when a parameter starts with :, it's trailing -> rest of the string is one big parameter
		if (msg[0] == ':')
		{
			this->_parameters.push_back(msg.substr(1));
			break ;
		}
		//else, parameters are separated by spaces, take substring until next space
		else if (msg.find(' ', 0) != std::string::npos)
		{
			this->_parameters.push_back(msg.substr(0, msg.find(' ', 0)));
			msg.erase(0, msg.find(' ', 0) + 1);
			msg = ft::skipSpaces(msg);
		}
		//if there is no more space, take the rest of the string as parameter
		else
		{
			this->_parameters.push_back(msg);
			break ;
		}
	}
	this->_isValid = true;
}

Message::~Message() {}

std::string	Message::getPrefix() const
{
	return this->_prefix;
}

std::string	Message::getCommand() const
{
	return this->_command;
}

std::vector<std::string>	Message::getParameters() const
{
	return this->_parameters;
}

bool	Message::isValid() const
{
	return this->_isValid;
}
