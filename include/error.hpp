#ifndef ERROR_HPP
# define ERROR_HPP

# include "irc.hpp"

# define ERR_NOSUCHNICK "401 <nickname> :No such nick/channel"

# define ERR_NONICKNAMEGIVEN "431 :No nickname given"

# define ERR_ERRONEUSNICKNAME "432 <nick> :Erroneus nickname"

# define ERR_NICKNAMEINUSE "433 <nick> :Nickname is already in use"

# define ERR_NEEDMOREPARAMS "461 <command> :Not enough parameters"

# define ERR_ALREADYREGISTRED "462 :You may not reregister"

#endif
