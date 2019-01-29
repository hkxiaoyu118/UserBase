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

	std::wstring StrGbkToUnicodeV2(const char *lpcszString)
	{
		int len = strlen(lpcszString);
		int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
		wchar_t *pUnicode = new wchar_t[unicodeLen + 1];
		memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
		std::wstring wString = (wchar_t *)pUnicode;
		delete[] pUnicode;
		return wString;
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

	std::string StrUtf8ToGbk(const std::string &str)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		unsigned short * wszGBK = new unsigned short[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)wszGBK, len);

		len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
		char *szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
		std::string strTemp(szGBK);
		delete[]szGBK;
		delete[]wszGBK;
		return strTemp;
	}

	std::string StrGbkToUtf8(const std::string &str)
	{
		int n = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		WCHAR *str1 = new WCHAR[n + 1];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str1, n);
		n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
		char *utf8Str = new char[n + 1];
		WideCharToMultiByte(CP_UTF8, 0, str1, -1, utf8Str, n, NULL, NULL);
		std::string result(utf8Str);
		delete[]utf8Str;
		delete[]str1;
		return result;
	}

	std::string &StdStrFormat(std::string & _str, const char * _Format, ...)
	{
		std::string tmp;

		va_list marker = NULL;
		va_start(marker, _Format);

		size_t num_of_chars = _vscprintf(_Format, marker);

		if (num_of_chars > tmp.capacity()) {
			tmp.resize(num_of_chars + 1);
		}

		vsprintf_s((char *)tmp.data(), tmp.capacity(), _Format, marker);

		va_end(marker);

		_str = tmp.c_str();
		return _str;
	}

	std::string StrGetRandomStr(unsigned int count)
	{
		char *characters = "abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ123456789";
		std::string randomStr = "";
		HCRYPTPROV hProvider;
		unsigned int charCount = strlen(characters);
		if (::CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		{
			for (unsigned int i = 0; i < count; i++)
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
			for (unsigned int i = 0; i < count; i++)
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

	void Byte2Hex(unsigned char bData, unsigned char hex[])
	{
		int high = bData / 16, low = bData % 16;
		hex[0] = (high < 10) ? ('0' + high) : ('A' + high - 10);
		hex[1] = (low < 10) ? ('0' + low) : ('A' + low - 10);
	}

	std::string StrCvtByteToString(unsigned char *buffer, unsigned int bufferSize)
	{
		std::string result = "";
		for (unsigned int i = 0; i < bufferSize; i++)
		{
			unsigned char hexBuf[3] = { 0 };
			Byte2Hex(buffer[i], hexBuf);
			result = result + (char *)hexBuf;
		}
		return result;
	}

	std::string StrTrim(std::string &str)
	{
		std::string::size_type pos = str.find_last_not_of(' ');
		if (pos != std::string::npos)
		{
			str.erase(pos + 1);
			pos = str.find_first_not_of(' ');
			if (pos != std::string::npos)
				str.erase(0, pos);
		}
		else
			str.erase(str.begin(), str.end());
		return str;
	}
}
