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
	std::string StrUnicodeToUtf8(const std::wstring &wstr);//unicode转utf8
	std::wstring StrUtf8ToUnicode(const std::string &str);//utf8转unicode
	std::string StrGetRandomStr(unsigned int count);//生成指定长度的随机字符串(真随机数生成的)
	std::string StrGetRandomStrEx(unsigned int count, char *characters);//通过指定的字符串生成随机的字符串(真随机数生成的)
}
