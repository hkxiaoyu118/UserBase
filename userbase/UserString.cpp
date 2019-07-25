#include "stdafx.h"
#include "UserString.h"
#include "ThirdParty/modp_b64/modp_b64.h"

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

	std::string BinToHex(const std::string& strBin, bool bIsUpper)
	{
		std::string strHex;
		strHex.resize(strBin.size() * 2);
		for (size_t i = 0; i < strBin.size(); i++)
		{
			uint8_t cTemp = strBin[i];
			for (size_t j = 0; j < 2; j++)
			{
				uint8_t cCur = (cTemp & 0x0f);
				if (cCur < 10)
				{
					cCur += '0';
				}
				else
				{
					cCur += ((bIsUpper ? 'A' : 'a') - 10);
				}
				strHex[2 * i + 1 - j] = cCur;
				cTemp >>= 4;
			}
		}

		return strHex;
	}

	std::string HexToBin(const std::string & strHex)
	{
		if (strHex.size() % 2 != 0)
		{
			return "";
		}

		std::string strBin;
		strBin.resize(strHex.size() / 2);
		for (size_t i = 0; i < strBin.size(); i++)
		{
			uint8_t cTemp = 0;
			for (size_t j = 0; j < 2; j++)
			{
				char cCur = strHex[2 * i + j];
				if (cCur >= '0' && cCur <= '9')
				{
					cTemp = (cTemp << 4) + (cCur - '0');
				}
				else if (cCur >= 'a' && cCur <= 'f')
				{
					cTemp = (cTemp << 4) + (cCur - 'a' + 10);
				}
				else if (cCur >= 'A' && cCur <= 'F')
				{
					cTemp = (cTemp << 4) + (cCur - 'A' + 10);
				}
				else
				{
					return "";
				}
			}
			strBin[i] = cTemp;
		}

		return strBin;
	}

	void StrLowerString(std::string& str)
	{
		if (str.empty())
			return;
		char* start = &str[0];
		char* end = start + str.length();
		for (; start < end; start++)
		{
			if (*start >= 'A' && *start <= 'Z')
				* start += 'a' - 'A';
		}
	}

	void StrLowerWString(std::wstring& str)
	{
		if (str.empty())
			return;
		wchar_t* start = &str[0];
		wchar_t* end = start + str.length();
		for (; start < end; start++)
		{
			if (*start >= L'A' && *start <= L'Z')
				* start += L'a' - L'A';
		}
	}

	void StrUpperString(std::string& str)
	{
		if (str.empty())
			return;
		char* start = &str[0];
		char* end = start + str.length();
		for (; start < end; start++)
		{
			if (*start >= 'a' && *start <= 'z')
				* start -= 'a' - 'A';
		}
	}

	void StrUpperWString(std::wstring& str)
	{
		if (str.empty())
			return;
		wchar_t* start = &str[0];
		wchar_t* end = start + str.length();
		for (; start < end; start++)
		{
			if (*start >= L'a' && *start <= L'z')
				* start -= L'a' - L'A';
		}
	}

	std::string StrMakeLowerString(const std::string& src)
	{
		std::string dest(src);
		StrLowerString(dest);
		return dest;
	}

	std::wstring StrMakeLowerWString(const std::wstring& src)
	{
		std::wstring dest(src);
		StrLowerWString(dest);
		return dest;
	}

	std::string StrMakeUpperString(const std::string& src)
	{
		std::string dest(src);
		StrUpperString(dest);
		return dest;
	}

	std::wstring StrMakeUpperWString(const std::wstring& src)
	{
		std::wstring dest(src);
		StrUpperWString(dest);
		return dest;
	}

	std::string BinaryToHexString(const void* binary, size_t length)
	{
		std::string output;
		static const unsigned char kHexChars[] = "0123456789abcdef";

		output.resize(length << 1);
		if (length == 0)
			return output;

		unsigned char* dst = reinterpret_cast<unsigned char*>(&output[0]);
		const unsigned char* src = reinterpret_cast<const unsigned char*>(binary);

		for (size_t i = 0; i < length; i++)
		{
			dst[i << 1] = kHexChars[src[i] >> 4];
			dst[(i << 1) + 1] = kHexChars[src[i] & 0xF];
		}
		return output;
	}

	std::string HexStringToBinary(const char* input, size_t length)
	{
		std::string output;
		output.resize(length >> 1);
		if (length == 0)
			return 0;

		const char* src = input;
		char* dst = &output[0];
		size_t i, size = output.size();

		for (i = 0; i < size; i++)
		{
			char h = StrHexCharToInt8(src[i << 1]);
			char l = StrHexCharToInt8(src[(i << 1) + 1]);
			if (l == -1 || h == -1)
				break;
			dst[i] = (h << 4) | l;
		}

		// i may be shorter than size when an error occurs
		output.resize(i);
		return output;
	}

	char StrHexCharToInt8(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		else if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;
		assert(0);	// not a hexadecimal char
		return -1;
	}


	void UTF8CreateLengthTable(unsigned table[256])
	{
		int c;

		for (c = 0; c < 256; c++)
		{
			if (c < 0x80)
				table[c] = 1;
			else if (0xBF < c && c < 0xE0)
				table[c] = 2;
			else if (0xDF < c && c < 0xF0)
				table[c] = 3;
			else if (0xEF < c && c < 0xF8)
				table[c] = 4;
			else
				table[c] = 0;
		}
	}

	bool StrValidateUTF8Stream(const void* stream, unsigned length)
	{
		/*
		 *	根据RFC3629（http://tools.ietf.org/html/rfc3629），
		 *	UTF-8流一个字符的第一个字节由0-4个数值为1的二进制位
		 *	这些位之后的第一位必须是0，当这些位的个数为0的时候表
		 *	示这个字符是ASCII，占用一个字节，除此之外表示这个字符
		 *	所占用的字节数；第二个字节开始，每个字节必须使用10的
		 *	二进制位开头，这样利于快速定位一个字符的起始字节，例如：
		 *
		 *	0XXXXXXX
		 *	110XXXXX 10XXXXXX
		 *	1110XXXX 10XXXXXX 10XXXXXX
		 *	11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
		 *
		 *	另，UTF-8理论上支持6字节长度，但是标准将其限定为4字节
		 */

		unsigned i, j, k;
		unsigned char* s = (unsigned char*)stream;
		static unsigned int table_created = 0;
		static unsigned int table[256];

		/* 保证多线程安全 */
		if (!table_created)
		{
			/* 利用lock-free的思想创建一模一样的表 */
			UTF8CreateLengthTable(table);
			/* 标记，之后的线程将不会重复创建该表 */
			table_created = 1;
		}

		/* 这里使用查表法是因为考虑到这个表会被放入CPU的Cache */

		for (i = 0; i < length;)
		{
			k = table[s[i]];
			if (k == 0)
				break;
			for (j = 1; j < k; j++)
				if (table[s[i + j]])
					return false;
			i += j;
		}

		return i == length;
	}

	bool StrValidateGB2312Stream(const void* stream, unsigned length)
	{
		/*
		 *	根据http://zh.wikipedia.org/zh-cn/Gb2312：
		 *	01-09区为特殊符号。
		 *	16-55区为一级汉字，按拼音排序。
		 *	56-87区为二级汉字，按部首／笔画排序。
		 *	10-15区及88-94区则未有编码。
		 *
		 *	每个汉字及符号以两个字节来表示。第一个字节称为“高位字节”，第二个字节称为“低位字节”。
		 *	“高位字节”使用了0xA1-0xF7（把01-87区的区号加上0xA0），“低位字节”使用了0xA1-0xFE（把01-94加上 0xA0）。
		 *	由于一级汉字从16区起始，汉字区的“高位字节”的范围是0xB0-0xF7，“低位字节”的范围是0xA1-0xFE，
		 *	占用的码位是 72*94=6768。其中有5个空位是D7FA-D7FE。
		 */

		unsigned char* s = (unsigned char*)stream;
		unsigned char* e = s + length;

		for (; s < e; s++)
		{
			if (*s < 0x80)
				continue;
			if (*s < 0xA1 || 0xE7 < *s)
				break;
			if (++s == e)
				return false;
			if (*s < 0xA1 || 0xFE < *s)
				break;
		}

		return s == e;
	}

	bool StrValidateGBKStream(const void* stream, unsigned length)
	{
		unsigned char* s = (unsigned char*)stream;
		unsigned char* e = s + length;

		for (; s < e; s++)
		{
			if (*s < 0x80)
				continue;
			if (*s < 0x81 || 0xFE < *s)
				break;
			if (++s == e)
				return false;
			if (*s < 0x40 || 0xFE < *s)
				break;
		}

		return s == e;
	}

	bool StrValidateBIG5Stream(const void* stream, unsigned length)
	{
		unsigned char* s = (unsigned char*)stream;
		unsigned char* e = s + length;

		for (; s < e; s++)
		{
			if (*s < 0x80)
				continue;
			if (*s < 0xA1 || 0xF9 < *s)
				break;
			if (++s == e)
				return false;
			if (*s < 0x40 || 0xFE < *s || (0x7E < *s && *s < 0xA1))
				break;
		}

		return s == e;
	}


	#define NOT_SPACE(x) ((x) != 0x20 && ((x) < 0 || 0x1d < (x)))

	template<typename CharType>
	void StringTrimT(std::basic_string<CharType>& output)
	{
		if (output.empty())
			return;
		size_t bound1 = 0;
		size_t bound2 = output.length();
		const CharType* src = output.data();

		for (; bound2 > 0; bound2--)
			if (NOT_SPACE(src[bound2 - 1]))
				break;

		for (; bound1 < bound2; bound1++)
			if (NOT_SPACE(src[bound1]))
				break;

		if (bound1 < bound2) {
			memmove((void*)src,
				src + bound1,
				sizeof(CharType) * (bound2 - bound1));
		}

		output.resize(bound2 - bound1);
	}

	template<typename CharType>
	void StringTrimLeftT(std::basic_string<CharType>& output)
	{
		size_t check = 0;
		size_t length = output.length();
		const CharType* src = output.data();

		for (; check < length; check++)
			if (NOT_SPACE(src[check]))
				break;

		output.erase(0, check);
	}

	template<typename CharType>
	void StringTrimRightT(std::basic_string<CharType>& output)
	{
		size_t length = output.length();
		const CharType* src = output.data();

		for (; length > 0; length--)
			if (NOT_SPACE(src[length - 1]))
				break;

		output.resize(length);
	}

	std::string& StrStringTrimLeft(std::string& input)
	{
		StringTrimLeftT<char>(input);
		return input;
	}

	std::string& StrStringTrimRight(std::string& input)
	{
		StringTrimRightT<char>(input);
		return input;
	}

	std::string& StrStringTrim(std::string& input) /* both left and right */
	{
		StringTrimT<char>(input);
		return input;
	}

	std::wstring& StrWStringTrimLeft(std::wstring& input)
	{
		StringTrimLeftT<wchar_t>(input);
		return input;
	}

	std::wstring& StrWStringTrimRight(std::wstring& input)
	{
		StringTrimRightT<wchar_t>(input);
		return input;
	}

	std::wstring& StrWStringTrim(std::wstring& input) /* both left and right */
	{
		StringTrimT<wchar_t>(input);
		return input;
	}

	template<typename CharType>
	size_t StringReplaceAllT(const std::basic_string<CharType>& find,
		const std::basic_string<CharType>& replace,
		std::basic_string<CharType>& output)
	{
		size_t find_length = find.size();
		size_t replace_length = replace.size();
		size_t offset = 0, endpos;
		size_t target = 0, found_pos;
		size_t replaced = 0;
		CharType* data_ptr;

		if (find.empty() || output.empty())
			return 0;

		/*
		 * to avoid extra memory reallocating,
		 * we use two passes to finish the task in the case that replace.size() is greater find.size()
		 */

		if (find_length < replace_length)
		{
			/* the first pass, count all available 'find' to be replaced  */
			for (;;)
			{
				offset = output.find(find, offset);
				if (offset == std::basic_string<CharType>::npos)
					break;
				replaced++;
				offset += find_length;
			}

			if (replaced == 0)
				return 0;

			size_t newsize = output.size() + replaced * (replace_length - find_length);

			/* we apply for more memory to hold the content to be replaced */
			endpos = newsize;
			offset = newsize - output.size();
			output.resize(newsize);
			data_ptr = &output[0];

			memmove((void*)(data_ptr + offset),
				(void*)data_ptr,
				(output.size() - offset) * sizeof(CharType));
		}
		else
		{
			endpos = output.size();
			offset = 0;
			data_ptr = const_cast<CharType*>(&output[0]);
		}

		/* the second pass,  the replacement */
		while (offset < endpos)
		{
			found_pos = output.find(find, offset);
			if (found_pos != std::basic_string<CharType>::npos)
			{
				/* move the content between two targets */
				if (target != found_pos)
					memmove((void*)(data_ptr + target),
					(void*)(data_ptr + offset),
						(found_pos - offset) * sizeof(CharType));

				target += found_pos - offset;

				/* replace */
				memcpy(data_ptr + target,
					replace.data(),
					replace_length * sizeof(CharType));

				target += replace_length;
				offset = find_length + found_pos;
				replaced++;
			}
			else
			{
				/* ending work  */
				if (target != offset)
					memcpy((void*)(data_ptr + target),
					(void*)(data_ptr + offset),
						(endpos - offset) * sizeof(CharType));
				break;
			}
		}

		if (replace_length < find_length)
			output.resize(output.size() - replaced * (find_length - replace_length));

		return replaced;
	}

	size_t StrStringReplaceAll(const std::string& find, const std::string& replace, std::string& output)
	{
		return StringReplaceAllT<char>(find, replace, output);
	}

	size_t StrWStringReplaceAll(const std::wstring& find, const std::wstring& replace, std::wstring& output)
	{
		return StringReplaceAllT<wchar_t>(find, replace, output);
	}

	template<typename CharType>
	int StringTokenizeT(const std::basic_string<CharType>& input,
		const std::basic_string<CharType>& delimitor,
		std::list<std::basic_string<CharType> >& output)
	{
		size_t token_begin;
		size_t token_end;

		output.clear();

		for (token_begin = token_end = 0; token_end != std::basic_string<CharType>::npos;)
		{
			token_begin = input.find_first_not_of(delimitor, token_begin);
			if (token_begin == std::basic_string<CharType>::npos)
				break;
			token_end = input.find_first_of(delimitor, token_begin + 1);
			output.push_back(input.substr(token_begin, token_end - token_begin));
			token_begin = token_end + 1;
		}

		return static_cast<int>(output.size());
	}

	std::list<std::string> StrStringTokenize(const char* input, const char* delimitor)
	{
		std::list<std::string> output;
		std::string input2(input);

		if (input2.empty())
			return output;

		char* token = strtok(&input2[0], delimitor);
		while (token != NULL)
		{
			output.push_back(token);
			token = strtok(NULL, delimitor);
		}

		return output;
	}

	std::list<std::wstring> StrWStringTokenize(const wchar_t* input, const wchar_t* delimitor)
	{
		std::list<std::wstring> output;
		std::wstring input2(input);

		if (input2.empty())
			return output;

#if defined(OS_WIN)	
		wchar_t* token = wcstok(&input2[0], delimitor);
		while (token != NULL)
		{
			output.push_back(token);
			token = wcstok(NULL, delimitor);
		}
#else
		wchar_t* ptr;
		wchar_t* token = wcstok(const_cast<wchar_t*>(&input2[0]), delimitor, &ptr);
		while (token != NULL)
		{
			output.push_back(token);
			token = wcstok(NULL, delimitor, &ptr);
		}
#endif

		return output;
	}

	bool StrBase64Encode(const std::string& input, std::string* output)
	{
		std::string temp;
		temp.resize(modp_b64_encode_len(input.size()));  // makes room for null byte

		// null terminates result since result is base64 text!
		int input_size = static_cast<int>(input.size());
		int output_size = modp_b64_encode(&(temp[0]), input.data(), input_size);
		if (output_size < 0)
			return false;

		temp.resize(output_size);  // strips off null byte
		output->swap(temp);
		return true;
	}

	bool StrBase64Decode(const std::string& input, std::string* output)
	{
		std::string temp;
		temp.resize(modp_b64_decode_len(input.size()));

		// does not null terminate result since result is binary data!
		int input_size = static_cast<int>(input.size());
		int output_size = modp_b64_decode(&(temp[0]), input.data(), input_size);
		if (output_size < 0)
			return false;

		temp.resize(output_size);
		output->swap(temp);
		return true;
	}
}
