#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "irc.hpp"

class Message {
	private:
		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_parameters;
		bool						_isValid;

	public:
		Message(std::string	msg);
		~Message();

		bool	isValid() const;
};

#endif
