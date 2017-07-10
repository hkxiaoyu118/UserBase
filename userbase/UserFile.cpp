#include "stdafx.h"
#include "UserFile.h"
#include "ThirdParty/MD5.h"
#include "UserString.h"

#pragma comment(lib,"Shlwapi.lib")

namespace ubase
{
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

	std::string FsGetFileMD5(std::string &filePath)
	{
		std::string strMd5 = "";
		std::string data;
		if (FsReadFile(filePath, data))
		{
			thirdparty::MD5_CTX md5;
			unsigned char md5Buffer[16] = { 0 };
			md5.MD5Update((unsigned char *)data.c_str(), data.length());
			md5.MD5Final(md5Buffer);
			strMd5 = ubase::StrCvtByteToString(md5Buffer, 16);
		}
		return strMd5;
	}

	std::string FsGetDesktopPath()
	{
		return FsGetCSIDLPath(CSIDL_DESKTOPDIRECTORY);
	}

	std::string FsGetDocumentPath()
	{
		return FsGetCSIDLPath(CSIDL_PERSONAL);
	}

	std::string FsGetProgramPath()
	{
		return FsGetCSIDLPath(CSIDL_PROGRAM_FILES);
	}

	std::string FsGetCSIDLPath(IN int nCSIDL)
	{
		std::string strPath;
		TCHAR szBuf[1025] = {0};
		if (::SHGetSpecialFolderPath(NULL, szBuf, nCSIDL, TRUE))
			strPath = szBuf;
		FsAddBackslash(strPath);
		return strPath;
	}

	std::string FsGetTempPath()
	{
		TCHAR szBuf[1025] = { 0 };
		::GetTempPath(1024, szBuf);
		std::string strPath(szBuf);
		FsAddBackslash(strPath);
		return strPath;
	}

	std::string FsGetWindowsPath()
	{
		return FsGetCSIDLPath(CSIDL_WINDOWS);
	}

	std::string FsGetSystem32Path()
	{
		return FsGetCSIDLPath(CSIDL_SYSTEM);
	}

	std::string FsGetProcessPath()
	{
		TCHAR szAppDir[1025];
		GetModuleFileName(NULL, szAppDir, 1024);
		std::string strPath(szAppDir);
		strPath = strPath.substr(0, strPath.rfind('\\') + 1);
		return strPath;
	}

	std::string FsGetProcessFullPath()
	{
		TCHAR szAppDir[1025];
		GetModuleFileName(NULL, szAppDir, 1024);
		std::string strFileName(szAppDir);
		return strFileName;
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

	std::string FsGetFileExt(IN const std::string& strFilePath)
	{
		std::string fileExt = "";
		size_t nPos = strFilePath.rfind('.');
		if (nPos != std::string::npos)
		{
			fileExt = strFilePath.substr(nPos + 1);
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

	void FsAddBackslash(IN OUT std::string& strDir)
	{
		if (strDir.empty())
			return;

		if (strDir.rfind("\\") != strDir.size())
			strDir += "\\";
	}

	void FsDelBackslash(IN OUT std::string& strDir)
	{
		if (strDir.empty())
			return;

		if (strDir.rfind("\\") == strDir.length() - 1)
		{
			strDir = strDir.substr(0, strDir.length() - 1);
		}
	}

	bool FsIsDir(IN const std::string& strPath)
	{
		if (!::PathFileExists(strPath.c_str()))
			return false;
		if (PathIsDirectory(strPath.c_str()))
			return true;
		return false;
	}

	bool FsIsFile(IN const std::string& strFilePath)
	{
		if (!::PathFileExists(strFilePath.c_str()))
			return false;
		if (!PathIsDirectory(strFilePath.c_str()))
			return true;
		return false;
	}

	__int64 FsGetFileSize(IN const std::string& strFileName)
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
					llModifyTime = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
				}
				CloseHandle(hFile);
			}
		}
		return llModifyTime;
	}

	LONGLONG FsGetCreateTime(IN const std::string& strFile)
	{
		LONGLONG llCreateTime = 0;
		FILETIME ftCreate, ftModify, ftAccess;
		if (FsIsFile(strFile))
		{
			HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify))
				{
					ULARGE_INTEGER ui;
					ui.LowPart = ftCreate.dwLowDateTime;
					ui.HighPart = ftCreate.dwHighDateTime;
					llCreateTime = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
				}
				CloseHandle(hFile);
			}
		}
		return llCreateTime;
	}

	LONGLONG FsGetAccessTime(IN const std::string& strFile)
	{
		LONGLONG llAccessTime = 0;
		FILETIME ftCreate, ftModify, ftAccess;
		if (FsIsFile(strFile))
		{
			HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify))
				{
					ULARGE_INTEGER ui;
					ui.LowPart = ftAccess.dwLowDateTime;
					ui.HighPart = ftAccess.dwHighDateTime;
					llAccessTime = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
				}
				CloseHandle(hFile);
			}
		}
		return llAccessTime;
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

	bool FsMoveFileEx(IN const std::string& strSrc, IN OUT std::string& strDst)
	{
		if (::MoveFileA(strSrc.c_str(), strDst.c_str()) == FALSE)
		{
			FsDeleteFileEx(strDst);
			if (::MoveFileA(strSrc.c_str(), strDst.c_str())==FALSE)
				return false;
		}
		return true;
	}

	bool FsForceCreateDir(IN const std::string& strDir)
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
}