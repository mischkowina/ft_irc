#ifndef ERROR_HPP
# define ERROR_HPP

# include "irc.hpp"

# define ERR_NOSUCHNICK "401 <nickname> :No such nick/channel"

# define ERR_NOSUCHSERVER "402 <server name> :No such server"

# define ERR_NOSUCHCHANNEL "403 <channel name> :No such channel"

# define ERR_CANNOTSENDTOCHAN "404 <channel name> :Cannot send to channel"

# define ERR_TOOMANYCHANNELS "405 <channel name> :You have joined too many channels"	//have joined the maximum number of allowed channels and they try to join another channel

# define ERR_TOOMANYTARGETS	"407 <target> :Duplicate recipients. No message delivered"

# define ERR_NOORIGIN "409 :No origin specified"

# define ERR_NORECIPIENT "411 :No recipient given (<command>)"

# define ERR_NOTEXTTOSEND	"412 :No text to send"

# define ERR_UNKNOWNCOMMAND "421 <command> :Unknown command"

# define ERR_NONICKNAMEGIVEN "431 :No nickname given"

# define ERR_ERRONEUSNICKNAME "432 <nick> :Erroneus nickname"

# define ERR_NICKNAMEINUSE	"433 <nick> :Nickname is already in use"

# define ERR_NOTONCHANNEL "442 <channel> :You're not on that channel"

# define ERR_USERONCHANNEL	"443 <user> <channel> :is already on channel" // - Returned when a client tries to invite a user to a channel they are already on.

# define ERR_NOTREGISTERED "451 :You have not registered"

# define ERR_NEEDMOREPARAMS "461 <command> :Not enough parameters"

# define ERR_ALREADYREGISTRED "462 :You may not reregister"

# define ERR_PASSWDMISMATCH "464 :Password incorrect"

# define ERR_KEYSET			"467 <channel> :Channel key already set"

# define ERR_CHANNELISFULL	"471 <channel> :Cannot join channel (+l)"

# define ERR_UNKNOWNMODE		"472 <char> :is unknown mode char to this server"//TBD -> wording

# define ERR_INVITEONLYCHAN	"473 <channel> :Cannot join channel (+i)"

# define ERR_BANNEDFROMCHAN	"474 <channel> :Cannot join channel (+b)"

# define ERR_BADCHANNELKEY	"475 <channel> :Cannot join channel (+k)"

# define ERR_NOPRIVILEGES	"481 :Permission Denied- You're not an IRC operator"

# define ERR_CHANOPRIVSNEEDED "482 <channel> :You're not channel operator"

#endif
