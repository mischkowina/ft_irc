#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

std::string return_banList(int v);
bool compDiff(std::string& str1, std::string& str2);

std::string userNick = "oli";
std::string userName = "USER1";
// std::string userHost = "host1.edu";
std::string userHost = "uni.edu";

std::string _banList0 = "*!*@*";
// std::string _banList1 = "mare!superuser@uni.edu";
std::string _banList1 = "oli!USER1@host1.edu";

std::string _banList2 = "*!*@*.edu";
std::string _banList3 = "*!*@uni.*";

std::string _banList4 = "oli*!*@*";
//std::string _banList4 = "o*!*@*";
std::string _banList5 = "xx*!*@*";

std::string _banList6 = "*i!*@*";
std::string _banList7 = "*a!*@*";

std::string _banList8 = "*!*1@*";
std::string _banList9 = "*!*9@*";
std::string _banList10 = "o*!X*@uni.*";
std::string _banList11 = "o*!U*@uni.*";

int main(int argc, char **argv) {
	if (argc != 2)
		return 42;
	std::string _BANLIST = return_banList(atoi(argv[1]));
	std::stringstream ss(_BANLIST);
	std::string token;
	while (std::getline(ss, token, ','))
	{
		std::string banNick, banUser, banHost;
		size_t pos1 = token.find('!');
		size_t pos2 = token.find('@');

		banNick = token.substr(0, pos1);
		banUser = token.substr(pos1 + 1, pos2 - pos1 - 1);
		banHost = token.substr(pos2 + 1);

		if (compDiff(userNick, banNick) == true && compDiff(userName, banUser)
			&& compDiff(userHost, banHost) == true)
		{
			std::cout << " compDiff: BANNED user : " << userNick << std::endl;
			std::cout << " _BANLIST : " << _BANLIST << std::endl;
			std::cout << " userID : " << userNick <<"!"<<userName<<"@"<<userHost<< std::endl;
		}
		else {
			std::cout << " _BANLIST : " << _BANLIST << std::endl;
			std::cout << " NOT banned userID : " << userNick <<"!"<<userName<<"@"<<userHost<< std::endl;
		}

	}
	return 0;
}

bool	compDiff(std::string& str1, std::string& str2)
{
	// std::cout << " ------- compDiff -----\n";
	// std::cout << "str1 : " << str1 << " " << " str2 : " << str2<< std::endl;
	
	if (str2[str2.size() - 1] == '*')
	{
		// std::cout << " __pos__ is : " << str2.size() - 1 << " and char : " << str2[str2.size() - 1] << std::endl;
		
		std::string sub1 = str2.substr(0, str2.size() - 1);		// uni.*
		// std::cout << " ::debug sub1 : " << sub1 << std::endl;
		// std::cout << " --debug str1 : " << str1 << std::endl;
		// std::cout << " --debug str2 : " << str2 << std::endl;
		size_t pos = str1.find(sub1);
		// std::string sub2 = str2.substr(0, str2.size());
			// std::cout << " --debug pos : " << pos << std::endl;
			// std::cout << " --debug sub2 : " << sub2 << std::endl;
		if (pos != std::string::npos) {
			// std::cout << " 2. -> BANNED user : " << str1 << std::endl;
			return true;
		}
	}
	else if (str2[0] == '*')
	{
		std::string sub1 = str2.substr(1);
		// std::cout << " --debug sub1 : " << sub1 << std::endl;
		// std::cout << " --debug str1 : " << str1 << std::endl;
		// std::cout << " --debug str2 : " << str2 << std::endl;
		size_t pos = str1.find(sub1);
		// std::string sub2 = str2.substr(0, str2.size());
			// std::cout << " --debug pos : " << pos << std::endl;
			// std::cout << " --debug sub2 : " << sub2 << std::endl;
		if (pos != std::string::npos) {
			// std::cout << " 3. -> BANNED user : " << str1 << std::endl;
			return true;
		}
	}
	return false;
	// std::cout << " ------- compDiff -----\n";
}

std::string return_banList(int v)
{
	// std::cout << "value is : " << v << std::endl;
	switch (v) {
		case 0:
			return _banList0;
		case 1:
			return _banList1;
		case 2:
			return _banList2;
		case 3:
			return _banList3;
		case 4:
			return _banList4;
		case 5:
			return _banList5;
		case 6:
			return _banList6;
		case 7:
			return _banList7;
		case 8:
			return _banList8;
		case 9:
			return _banList9;
		case 10:
			return _banList10;
		case 11:
			return _banList11;
		default:
			std::cerr << "invalid option" << std::endl;
			return std::string ("none");
		}
	return std::string ("nonexistent");
}
