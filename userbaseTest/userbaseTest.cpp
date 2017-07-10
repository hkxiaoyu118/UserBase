// userbaseTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <Shlwapi.h>
#include <io.h>
#include "../userbase/ThirdParty/MD5.h"
#include "../userbase/UserFile.h"

#pragma comment(lib,"Shlwapi.lib")

bool FsReadFile(const std::string &filePath, std::string &data)
{
	bool result = false;
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD lowSize, readCount = 0;
		lowSize = GetFileSize(hFile, NULL);
		data.resize(lowSize);
		if (ReadFile(hFile, (char *)data.c_str(), lowSize, &readCount, NULL))
		{
			if (readCount == lowSize)
				result = true;
		}
		CloseHandle(hFile);
	}
	return result;
}

bool FsWriteFile(const std::string &filePath, std::string &data)
{
	bool result = false;
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD writeCount = 0;
		if (WriteFile(hFile, (char *)data.c_str(), data.length(), &writeCount, NULL))
		{
			if (writeCount == data.length())
				result = true;
		}
		CloseHandle(hFile);
	}
	return result;
}

void Byte2Hex(unsigned char bData, unsigned char hex[])
{
	int high = bData / 16, low = bData % 16;
	hex[0] = (high < 10) ? ('0' + high) : ('A' + high - 10);
	hex[1] = (low < 10) ? ('0' + low) : ('A' + low - 10);
}

std::string CvtByteToString(unsigned char *buffer, unsigned int bufferSize)
{
	std::string result = "";
	for (int i = 0; i < bufferSize; i++)
	{
		unsigned char hexBuf[3] = { 0 };
		Byte2Hex(buffer[i], hexBuf);
		result = result + (char *)hexBuf;
	}
	return result;
}

int main()
{
	std::string filePath = "D:\\档案\\周报\\周报20170629-孙小雨.txt";
	std::string md5Str = ubase::FsGetFileMD5(filePath);
    return 0;
}

