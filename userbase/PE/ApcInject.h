#ifndef _APC_INJECT_H_
#define _APC_INJECT_H_


#include <Windows.h>
#include <TlHelp32.h>

//APC注入依赖于目标进程的API操作,因此通用性不好,大部分情况下不建议使用
BOOL ApcInjectDll(char *pszProcessName, char *pszDllName);// APC注入

#endif