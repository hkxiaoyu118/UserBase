#include "SearchUserInfo.h"

namespace REGEX
{
	std::string SearchEmail(std::string data)
	{
		std::string result;
		std::regex reg(g_mailRetexStr);
		std::smatch r;
		if (std::regex_search(data, r, reg) == true)
		{
			result = r.str();
		}
		return result;
	}

	std::string SearchChinaTel(std::string data)
	{
		std::string result;
		std::regex reg(g_chinaTelRegexStr);
		std::smatch r;
		if (std::regex_search(data, r, reg) == true)
		{
			result = r.str();
		}
		return result;
	}
}