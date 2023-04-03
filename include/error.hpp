#ifndef ERROR_HPP
# define ERROR_HPP

# include "irc.hpp"

# define ERR_NOSUCHNICK "401 <nickname> :No such nick/channel"

# define ERR_NOSUCHSERVER "402 <server name> :No such server"

# define ERR_NOORIGIN "409 :No origin specified"

# define ERR_NORECIPIENT "411 :No recipient given (<command>)"

# define ERR_NOTEXTTOSEND " 412 :No text to send"

# define ERR_UNKNOWNCOMMAND "421 <command> :Unknown command"

# define ERR_NONICKNAMEGIVEN "431 :No nickname given"

# define ERR_ERRONEUSNICKNAME "432 <nick> :Erroneus nickname"

# define ERR_NICKNAMEINUSE "433 <nick> :Nickname is already in use"

# define ERR_NOTREGISTERED "451 :You have not registered"

# define ERR_NEEDMOREPARAMS "461 <command> :Not enough parameters"

# define ERR_ALREADYREGISTRED "462 :You may not reregister"

# define ERR_PASSWDMISMATCH "464 :Password incorrect"

# define ERR_NOPRIVILEGES "481 :Permission Denied- You're not an IRC operator"

#endif
