#pragma once
#include <ShlObj.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>


BOOL PsCreateOneLink(LPCTSTR  lpszExePath, LPCTSTR lpszLnkPath, int iIcon); //创建一个快捷方式
void PsCreateDesktopLink(LPCTSTR szExePath, LPCTSTR szLinkName, int iIcon);	//创建桌面快捷方式