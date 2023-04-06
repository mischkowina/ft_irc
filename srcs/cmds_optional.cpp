#include "cmds.hpp"

void	away(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.setAwayMsg("");
		Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(client.getNick());
		it->second.setAwayMsg("");
		client.sendErrMsg(server, RPL_UNAWAY, NULL);
	}
	else
	{
		client.setAwayMsg(parameters[0]);
		Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(client.getNick());
		it->second.setAwayMsg(parameters[0]);
		client.sendErrMsg(server, RPL_NOWAWAY, NULL);
	}
}

void	die(Server *server, Client &client, Message& msg)
{
	(void)msg;
	
	if (client.getIsOperator() == false)
	{
		client.sendErrMsg(server, ERR_NOPRIVILEGES, NULL);
		return ;
	}

	server->setHasDied(true);
}

void	horoscope(Server *server, Client &client, std::string msg)
{
	Client	horoscope(-1, true);
	std::vector<std::string> userdata;
	userdata.push_back("bot");
	userdata.push_back("0");
	userdata.push_back("0");
	userdata.push_back("0");
	horoscope.setUserData(userdata);
	sockaddr_in serverAddr = server->getServerAddr();
	horoscope.setIP(&serverAddr);
	horoscope.setNick("horoscope");

	
	if (msg == "AQUARIUS")
		client.sendMsg(horoscope, AQUARIUS, "PRIVMSG");
	else if (msg == "PISCES")
		client.sendMsg(horoscope, PISCES, "PRIVMSG");
	else if (msg == "ARIES")
		client.sendMsg(horoscope, ARIES, "PRIVMSG");
	else if (msg == "TAURUS")
		client.sendMsg(horoscope, TAURUS, "PRIVMSG");
	else if (msg == "GEMINI")
		client.sendMsg(horoscope, GEMINI, "PRIVMSG");
	else if (msg == "CANCER")
		client.sendMsg(horoscope, CANCER, "PRIVMSG");
	else if (msg == "LEO")
		client.sendMsg(horoscope, LEO, "PRIVMSG");
	else if (msg == "VIRGO")
		client.sendMsg(horoscope, VIRGO, "PRIVMSG");
	else if (msg == "LIBRA")
		client.sendMsg(horoscope, LIBRA, "PRIVMSG");
	else if (msg == "SCORPIO")
		client.sendMsg(horoscope, SCORPIO, "PRIVMSG");
	else if (msg == "SAGITTARIUS")
		client.sendMsg(horoscope, SAGITTARIUS, "PRIVMSG");
	else if (msg == "CAPRICORN")
		client.sendMsg(horoscope, CAPRICORN, "PRIVMSG");
	else
		client.sendMsg(horoscope, DEFAULT, "PRIVMSG");
}
