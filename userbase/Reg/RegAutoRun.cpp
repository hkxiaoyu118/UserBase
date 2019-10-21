#include "RegAutoRun.h"

BOOL RegAutoRunCurrentUser(char *lpszFileName, char *lpszValueName)
{
	//默认权限
	HKEY hKey;
	//打开注册表键
	if (ERROR_SUCCESS != ::RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey))
	{
		return FALSE;
	}
	//修改注册表值，实现开机自启
	if (ERROR_SUCCESS != ::RegSetValueExA(hKey, lpszValueName, 0, REG_SZ, (BYTE *)lpszFileName, (1 + ::lstrlen(lpszFileName))))
	{
		::RegCloseKey(hKey);
		return FALSE;
	}
	//关闭注册表键
	::RegCloseKey(hKey);

	return TRUE;
}


BOOL RegAuotRunLocalMachine(char *lpszFileName, char *lpszValueName)
{
	//管理员权限
	HKEY hKey;
	//打开注册表键
	if (ERROR_SUCCESS != ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey))
	{
		return FALSE;
	}
	//修改注册表值，实现开机自启
	if (ERROR_SUCCESS != ::RegSetValueExA(hKey, lpszValueName, 0, REG_SZ, (BYTE *)lpszFileName, (1 + ::lstrlen(lpszFileName))))
	{
		::RegCloseKey(hKey);
		return FALSE;
	}
	//关闭注册表键
	::RegCloseKey(hKey);

	return TRUE;
}