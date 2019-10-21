#ifndef _REG_AUTORUN_H_
#define _REG_AUTORUN_H_

/*
	以注册表方式实现自启动的各种方法
*/

#include <Windows.h>

/*
	HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run
	//不需要管理员身份
*/
BOOL RegAutoRunCurrentUser(char *lpszFileName, char *lpszValueName);

/*
	HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run
	//需要管理员身份
*/
BOOL RegAuotRunLocalMachine(char *lpszFileName, char *lpszValueName);


#endif