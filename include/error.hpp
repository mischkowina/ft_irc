#ifndef ERROR_HPP
# define ERROR_HPP

# include "irc.hpp"

/* commands */

#define ERR_UNKNOWNCOMMAND	"<command> :Unknown command"
/* - Returned to a registered client to indicate that the
	 command sent is unknown by the server. */

/* user */

# define ERR_NOSUCHNICK		"401 <nickname> :No such nick/channel"

# define ERR_NORECIPIENT	"411 :No recipient given (<command>)"

# define ERR_NOTEXTTOSEND	" 412 :No text to send"

# define ERR_NONICKNAMEGIVEN	"431 :No nickname given"

# define ERR_ERRONEUSNICKNAME	"432 <nick> :Erroneus nickname"

# define ERR_NICKNAMEINUSE	"433 <nick> :Nickname is already in use"

# define ERR_NEEDMOREPARAMS	"461 <command> :Not enough parameters"

# define ERR_ALREADYREGISTRED	"462 :You may not reregister"

/* channel */

#define ERR_KEYSET			"<channel> :Channel key already set"
#define ERR_CHANNELISFULL	"<channel> :Cannot join channel (+l)"
#define ERR_INVITEONLYCHAN	"<channel> :Cannot join channel (+i)"
#define ERR_BANNEDFROMCHAN	"<channel> :Cannot join channel (+b)"
#define ERR_BADCHANNELKEY	"<channel> :Cannot join channel (+k)"
#define ERR_NOSUCHCHANNEL	"<channel name> :No such channel"	// 403 code
#define ERR_TOOMANYCHANNELS	"<channel name> :You have joined too many channels"	//have joined the maximum number of allowed channels and they try to join another channel
#define RPL_NOTOPIC			"<channel> :No topic is set"
#define ERR_USERONCHANNEL	"<user> <channel> :is already on channel" // - Returned when a client tries to invite a user to a channel they are already on.

#define ERR_CANNOTSENDTOCHAN	"<channel name> :Cannot send to channel"
/* - Sent to a user who is either (a) not on a channel which is mode +n or (b) not a chanop (or mode +v) on
	a channel which has mode +m set and is trying to send a PRIVMSG message to that channel. */
				  



// WIP --> look at the rest of the err types

/* mode */

#define ERR_UNKNOWNMODE		"<char> :is unknown mode char to me"
#define ERR_NOPRIVILEGES	":Permission Denied- You're not an IRC operator"
/* - Any command requiring operator privileges to operate
				  must return this error to indicate the attempt was
				  unsuccessful. */

#define ERR_CHANOPRIVSNEEDED	"<channel> :You're not channel operator"
/*- Any command requiring 'chanop' privileges (such as
				  MODE messages) must return this error if the client
				  making the attempt is not a chanop on the specified
				  channel. */

#define ERR_TOOMANYTARGETS	"<target> :Duplicate recipients. No message delivered"
/* - Returned to a client which is attempting to send a
				  PRIVMSG/NOTICE using the user@host destination format
				  and for a user@host which has several occurrences. */

// #define ERR_NEEDMOREPARAMS	""

#endif
