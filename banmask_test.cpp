#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

std::string return_banList(int v);
bool compDiff(std::string& str1, std::string& str2);

std::string userNick = "oli";
std::string userName = "USER1";
std::string userHost = "host1";

std::string _banList0 = "*!*@*";
// std::string _banList1 = "mare!superuser@uni.edu";
std::string _banList1 = "oli!USER1@host1";

std::string _banList2 = "*!*@*.edu";
std::string _banList3 = "*!*@uni.*";

std::string _banList4 = "oli*!*@*";
//std::string _banList4 = "o*!*@*";
std::string _banList5 = "xx*!*@*";

std::string _banList6 = "*i!*@*";
std::string _banList7 = "*a!*@*";

std::string _banList8 = "*!*1@*";
std::string _banList9 = "*!*9@*";

int main(int argc, char **argv) {

	std::stringstream ss(return_banList(atoi(argv[1])));
	std::string token;
	while (std::getline(ss, token, ','))
	{
		std::string banNick, banUser, banHost;
		size_t pos1 = token.find('!');
		size_t pos2 = token.find('@');

		banNick = token.substr(0, pos1);
		banUser = token.substr(pos1 + 1, pos2 - pos1 - 1);
		banHost = token.substr(pos2 + 1);
		if ((banNick == "*" && banUser == "*" && banHost == "*")
			|| (banNick == userNick && banUser == userName && banHost == userHost))
		{
			std::cout << " 1. BANNED user : " << userNick << std::endl;
		}

		compDiff(userNick, banNick);
		compDiff(userName, banUser);
		compDiff(userHost, banHost);

		std::cout << "//debug: " << banNick << std::endl;
		std::cout << "//debug: " << banUser << std::endl;
		std::cout << "//debug: " << banHost << std::endl;
	}
	return 0;
}

bool	compDiff(std::string& str1, std::string& str2)
{
	if (str1[str1.size() - 1] == '*')
	{
		std::string sub1 = str1.substr(0, str1.find('*'));
		std::string sub2 = str2.substr(0, str2.find('*'));
		if (sub1 == sub2) {
			std::cout << " 2. BANNED user : " << str1 << std::endl;
			return true;
		}
	}
	else if (str1[0] == '*')
	{
		std::string sub1 = str1.substr(1, str1.size());
		std::string sub2 = str2.substr(1, str2.size());
			std::cout << "sub1 : " << sub1 << std::endl;
			std::cout << "sub2 : " << sub2 << std::endl;
		if (sub1 == sub2) {
			std::cout << " 3. BANNED user : " << str1 << std::endl;
			return true;
		}
	}
	return false;
}

std::string return_banList(int v)
{
	std::cout << "value is : " << v << std::endl;
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
		default:
			std::cerr << "invalid option" << std::endl;
			return std::string ("none");
		}
	return std::string ("nonexistent");
}

