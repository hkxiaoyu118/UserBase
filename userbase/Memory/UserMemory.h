#pragma once
#include <windows.h>
#include <process.h>
#include <Psapi.h>

namespace ubase
{
	MODULEINFO MmGetModuleInfo(char *szModule);//获取进程对应模块的信息
	DWORD MmFindPattern(char *module, char *pattern, char *mask);//查找符合对应模式的内存地址（例子:mask XXX?XXX??XX其中?代表模糊匹配）
	void MmWriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum);//写入当前进程空间内存
	void MmReadFromMemory(uintptr_t addressToRead, char* varToWriteTo, int byteNum);//读取当前进程空间内存
	ULONG_PTR MmFindImageBase(HANDLE hProc, LPSTR lpCommandLine);//从指定进程中找到对应模块的基地址(能够在进程刚创建还没有运行主线程之前调用(进程只有ntdll和exe模块加载时))
}
