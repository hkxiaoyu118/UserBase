#pragma once
#include <iostream>
#include <string>
#include <codecvt>
#include <windows.h>
#include <assert.h>
#include <Wincrypt.h>

namespace ubase
{
	std::string StrUnicodeToGbk(const std::wstring &wstr);//unicode转gbk
	std::wstring StrGbkToUnicode(const std::string &str);//gbk转unicode
	std::wstring StrGbkToUnicodeV2(const char *lpcszString);//gbk转unicode版本2(用的windows的api函数)
	std::string StrUnicodeToUtf8(const std::wstring &wstr);//unicode转utf8
	std::wstring StrUtf8ToUnicode(const std::string &str);//utf8转unicode
	std::string StrUtf8ToGbk(const std::string &str);//utf8转gbk
	std::string StrGbkToUtf8(const std::string &str);//gbk转utf8
	std::string &StdStrFormat(std::string & _str, const char * _Format, ...);//std::string版本的sprintf函数
	std::string StrGetRandomStr(unsigned int count);//生成指定长度的随机字符串(真随机数生成的)
	std::string StrGetRandomStrEx(unsigned int count, char *characters);//通过指定的字符串生成随机的字符串(真随机数生成的)
	void Byte2Hex(unsigned char bData, unsigned char hex[]);//字符转16进制显示
	std::string StrCvtByteToString(unsigned char *buffer, unsigned int bufferSize);//将内存字符以16进制的形式表现出来
	std::string StrTrim(std::string &str);//去掉std::string字符串左右两边的空格
	std::string BinToHex(const std::string& strBin, bool bIsUpper = false);
	std::string HexToBin(const std::string& strHex);
}
