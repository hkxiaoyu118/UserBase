#pragma once
#include <iostream>
#include <string>
#include <codecvt>
#include <list>
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
	void StrLowerString(std::string& str);//将string转化为小写
	void StrLowerWString(std::wstring& str);//将wstring转换为小写
	void StrUpperString(std::string& str);//将wstring转化为大写
	void StrUpperWString(std::wstring& str);//将wstring转化为大写
	std::string StrMakeLowerString(const std::string& src);//将string转换为小写,不改变源字符串的值
	std::wstring StrMakeLowerWString(const std::wstring& src);//将wstring转换为小写,不改变源字符串的值
	std::string StrMakeUpperString(const std::string& src);//将string转换为大写,不改变源字符串的值
	std::wstring StrMakeUpperWString(const std::wstring& src);//将wstring转换为大写,不改变源字符串的值
	std::string BinaryToHexString(const void* binary, size_t length);//将二进制转化为字符串
	std::string HexStringToBinary(const char* input, size_t length);//将字符串转化为二进制
	char StrHexCharToInt8(char c);//16禁止字符转数字
	bool StrValidateUTF8Stream(const void* stream, unsigned length);//判断字节流是否是UTF8格式
	bool StrValidateGB2312Stream(const void* stream, unsigned length);//判断字节流是否是GB2312格式
	bool StrValidateGBKStream(const void* stream, unsigned length);//判断字节流是否是GBK格式
	bool StrValidateBIG5Stream(const void* stream, unsigned length);//判断字节流是否是BIG5(大五码)格式
	std::string& StrStringTrimLeft(std::string& input);//将string类型字符串左边去空格
	std::string& StrStringTrimRight(std::string& input);//将string类型字符串右边去空格
	std::string& StrStringTrim(std::string& input);//将string类型字符串两边去空格
	std::wstring& StrWStringTrimLeft(std::wstring& input);//将wstring类型字符串左边去空格
	std::wstring& StrWStringTrimRight(std::wstring& input);//将wstring类型字符串右边去空格
	std::wstring& StrWStringTrim(std::wstring& input);//将wstring类型字符串两边去空格
	size_t StrStringReplaceAll(const std::string& find, const std::string& replace, std::string& output);//替换string类型字符串中的所有文本
	size_t StrWStringReplaceAll(const std::wstring& find, const std::wstring& replace, std::wstring& output);//替换wstring类型字符串中的所有文本
	std::list<std::string> StrStringTokenize(const char* input, const char* delimitor);//找到string类型字符串中所有通过delimitor标识分割的元素
	std::list<std::wstring> StrWStringTokenize(const wchar_t* input, const wchar_t* delimitor);//找到wstring类型字符串中所有通过delimitor标识分割的元素

	bool StrBase64Encode(const std::string& input, std::string* output);//base64编码
	bool StrBase64Decode(const std::string& input, std::string* output);//base64解码
}
