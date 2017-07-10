#include "stdafx.h"
#include "UserString.h"

#pragma comment(lib, "advapi32.lib")

namespace ubase
{
	std::string StrUnicodeToGbk(const std::wstring &wstr)
	{
		std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvtGbk(
			new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));
		std::string gbkStr = cvtGbk.to_bytes(wstr);
		return gbkStr;
	}

	std::wstring StrGbkToUnicode(const std::string &str)
	{
		std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvtGbk(
			new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));
		std::wstring unicoStr = cvtGbk.from_bytes(str);
		return unicoStr;
	}

	std::string StrUnicodeToUtf8(const std::wstring &wstr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvtUtf8;
		std::string utf8Str = cvtUtf8.to_bytes(wstr);
		return utf8Str;
	}

	std::wstring StrUtf8ToUnicode(const std::string &str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvtUtf8;
		std::wstring unicoStr = cvtUtf8.from_bytes(str);
		return unicoStr;
	}

	std::string StrGetRandomStr(unsigned int count)
	{
		char *characters = "abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ123456789";
		std::string randomStr = "";
		HCRYPTPROV hProvider;
		unsigned int charCount = strlen(characters);
		if (::CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		{
			for (int i = 0; i < count; i++)
			{
				BYTE buffer[4] = { 0 };
				char randBuffer[2] = { 0 };
				unsigned int valueNum = 0;
				DWORD ret = ::CryptGenRandom(hProvider, 4, buffer);
				valueNum = *(unsigned short*)buffer;
				sprintf(randBuffer, "%c", characters[valueNum % charCount]);
				randomStr += randBuffer;
			}
			::CryptReleaseContext(hProvider, 0);
		}
		return randomStr;
	}

	std::string StrGetRandomStrEx(unsigned int count, char *characters)
	{
		if (characters == NULL || strlen(characters) == 0)
		{
			return StrGetRandomStr(count);
		}

		std::string randomStr = "";
		HCRYPTPROV hProvider;
		unsigned int charCount = strlen(characters);
		if (::CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		{
			for (int i = 0; i < count; i++)
			{
				BYTE buffer[4] = { 0 };
				char randBuffer[2] = { 0 };
				unsigned int valueNum = 0;
				DWORD ret = ::CryptGenRandom(hProvider, 4, buffer);
				valueNum = *(unsigned short*)buffer;
				sprintf(randBuffer, "%c", characters[valueNum % charCount]);
				randomStr += randBuffer;
			}
			::CryptReleaseContext(hProvider, 0);
		}
		return randomStr;
	}
}
