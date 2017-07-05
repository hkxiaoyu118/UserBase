// userbaseTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <windows.h>
#include <Shlwapi.h>
#include <io.h>

#pragma comment(lib,"Shlwapi.lib")

std::string FsGetFileNameNoExt(IN const std::string& strFilePath)
{
	std::string fileName = "";
	size_t nPosBegin = strFilePath.rfind('\\');
	size_t nPosEnd = strFilePath.rfind('.');
	if (nPosBegin != std::string::npos && nPosEnd != std::string::npos && nPosEnd > nPosBegin)
	{
		fileName = strFilePath.substr(nPosBegin + 1, nPosEnd - nPosBegin - 1);
	}
	return fileName;
}

std::string FsGetFileName(IN const std::string& strFilePath)
{
	std::string fileName = "";
	size_t nPos = strFilePath.rfind('\\');
	if (nPos != std::string::npos)
	{
		fileName = strFilePath.substr(nPos + 1);
	}
	return fileName;
}

std::string GetFileExt(IN const std::string& strFile)
{
	std::string fileExt = "";
	size_t nPos = strFile.rfind('.');
	if (nPos != std::string::npos)
	{
		fileExt = strFile.substr(nPos + 1);
	}
	return fileExt;
}

std::string FsGetFilePath(IN const std::string& strFilePath)
{
	std::string filePath = "";
	size_t nPos = strFilePath.rfind('\\');
	if (nPos != std::string::npos)
	{
		filePath = strFilePath.substr(0, nPos + 1);
	}
	return filePath;
}

void FsDelFillBackslash(IN OUT std::string& strDir)
{
	if (strDir.empty())
		return;

	if (strDir.rfind("\\") == strDir.length() - 1)
	{
		strDir = strDir.substr(0, strDir.length() - 1);
	}
}

bool IsDir(IN const std::string& strPathFile)
{
	if (!::PathFileExists(strPathFile.c_str()))
		return false;
	if (PathIsDirectory(strPathFile.c_str()))
		return true;
	return false;
}

bool IsFile(IN const std::string& strFilePath)
{
	if (!::PathFileExists(strFilePath.c_str()))
		return false;
	if (!PathIsDirectory(strFilePath.c_str()))
		return true;
	return false;
}

__int64 GetFileLength(IN const std::string& strFileName)
{
	__int64 nLen = 0;
	if (!::PathFileExists(strFileName.c_str()))
		return nLen;

	HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER llSize = { 0 };
		if (GetFileSizeEx(hFile, &llSize))
		{
			nLen = llSize.QuadPart;
		}
		CloseHandle(hFile);
	}
	return nLen;
}

bool FsIsFile(IN const std::string& strFilePath)
{
	if (!::PathFileExists(strFilePath.c_str()))
		return false;
	if (!PathIsDirectory(strFilePath.c_str()))
		return true;
	return false;
}

LONGLONG FsGetModiftyTime(IN const std::string& strFile)
{
	LONGLONG llModifyTime = 0;
	FILETIME ftCreate, ftModify, ftAccess;
	if (FsIsFile(strFile))
	{
		HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify))
			{
				ULARGE_INTEGER ui;
				ui.LowPart = ftModify.dwLowDateTime;
				ui.HighPart = ftModify.dwHighDateTime;
				llModifyTime= ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
			}
			CloseHandle(hFile);
		}
	}
	return llModifyTime;
}

void FsAddBackslash(IN OUT std::string& strDir)
{
	if (strDir.empty())
		return;

	if (strDir.rfind("\\") != strDir.size())
		strDir += "\\";
}

bool CreateDir(IN const std::string& strDir)
{
	if (::PathFileExists(strDir.c_str()))
		return TRUE;

	std::string strPath = strDir;

	FsAddBackslash(strPath);

	size_t nPos = strPath.find('\\', 0);
	while (nPos != std::string::npos)
	{
		std::string strSubDir = strPath.substr(0, nPos);
		if (!::PathFileExists(strSubDir.c_str()))
			if (!::CreateDirectory(strSubDir.c_str(), NULL))
				return false;

		nPos = strPath.find('\\', nPos + 1);
	}
	return true;
}

bool FsDeleteFileEx(IN const std::string& strFileName)
{
	if (::DeleteFile(strFileName.c_str()) == FALSE)
	{
		::SetFileAttributes(strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
		if (::DeleteFile(strFileName.c_str()) == FALSE)
			return false;
	}
	return true;
}

void FsForceDeleteDir(IN const std::string& strDir)
{
	if (!::PathFileExists(strDir.c_str()))
		return;

	BOOL bRet = TRUE;
	std::string strPath(strDir);
	FsAddBackslash(strPath);
	std::string strFindDir = strPath + "*.*";
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = ::FindFirstFile(strFindDir.c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	while (bRet)
	{
		if (findFileData.cFileName[0] != '.')
		{
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string subStrSubPath = strPath + findFileData.cFileName;
				FsForceDeleteDir(subStrSubPath);
			}
			else
			{
				std::string fileName = strPath + findFileData.cFileName;
				FsDeleteFileEx(fileName);
			}
		}
		bRet = ::FindNextFile(hFind, &findFileData);
	}

	if (!::RemoveDirectory(strDir.c_str()))
	{
		::SetFileAttributes(strDir.c_str(), FILE_ATTRIBUTE_NORMAL);
		::RemoveDirectory(strDir.c_str());
	}

	::FindClose(hFind);
	return;
}

int main()
{
	std::string filePath = "C:\\Users\\Administrator\\Desktop\\hkxiaoyu118";
	FsForceDeleteDir(filePath);
    return 0;
}

