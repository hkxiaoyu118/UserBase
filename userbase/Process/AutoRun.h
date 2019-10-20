#pragma once
#include <ShlObj.h>
#include <tchar.h>
#include <windows.h>


BOOL PsCreateLink(LPCTSTR  lpszExePath, CONST WCHAR* lpWzLinkPath, int iIcon = 0); //创建一个快捷方式
void PsCreateDesktopLink(LPCTSTR szExePath, LPCTSTR szLinkName, int iIcon = 0);	//创建桌面快捷方式
BOOL PsAutoRunStartup(CONST CHAR* szSrcFilePath, CONST CHAR* szLinkName);//向Startup文件夹添加快捷方式实现程序开机启动