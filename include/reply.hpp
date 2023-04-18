#ifndef REPLY_HPP
# define REPLY_HPP

# include "irc.hpp"

# define RPL_WELCOME	"001 :Welcome to the Internet Relay Network <nick!user@host>"

# define RPL_YOURHOST	"002 :Your host is <servername>, running version ft_irc 1.0"

# define RPL_CREATED	"003 :This server was created 2023-03-29"

# define RPL_MYINFO		"004 :<servername> ft_irc 1.0 io opsitnmlbvk"

# define RPL_UMODEIS	"221 <user mode string>"

# define RPL_AWAY		"301 <nick> :<away message>"

# define RPL_UNAWAY		"305 :You are no longer marked as being away"

# define RPL_NOWAWAY	"306 :You have been marked as being away"

# define RPL_LIST		"322 <channel> <# visible>:<topic>"

# define RPL_LISTEND	"323 :End of /LIST"

# define RPL_CHANNELMODEIS "324: <channel> <mode> <mode params>"

# define RPL_NOTOPIC	"331 <channel> :No topic is set"

# define RPL_TOPIC		"332 <channel> :<topic>"

# define RPL_INVITING	"341 <channel> <nick>"

# define RPL_NAMREPLY	"353 <channel> :<list of names>"

# define RPL_ENDOFNAMES	"366 <channel> :End of NAMES list"

# define RPL_BANLIST	"367 <channel> <banid>"

# define RPL_ENDOFBANLIST	"368 <channel> :End of channel ban list"

# define RPL_MOTDSTART	"375 :- <server> Message of the day - "

# define RPL_MOTD		"372 :- <text>"

# define RPL_ENDOFMOTD	"376 :End of MOTD command"

# define RPL_YOUREOPER	"381 :You are now an IRC operator"

#endif
