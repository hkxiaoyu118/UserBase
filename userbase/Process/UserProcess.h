#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <Psapi.h>
#include <process.h>
#include <TlHelp32.h>

namespace ubase
{
	bool PsEnableDebugPriv();//提升当前进程权限函数("SeDebugPrivilege"读、写控制权限)
	bool PsEnableShutdownPriv();//提升当前进程权限函数("SeShutdownPrivilege"关机权限)
	bool PsEnableBackupPriv();//提升当前进程权限函数("SeBackupPrivilege"备份数据权限)
	bool PsEnableRestorePriv();//提升当前进程权限函数("SeRestorePrivilege"恢复数据权限)
	bool PsGetPidsByProcessName(std::vector<DWORD> &vDwPids, const std::string &processName);//通过进程名称获取进程pid,进程名不区分大小写
	bool PsGetAllProcessName(std::vector<std::string> &vsProcessName);//获取所有的进程的名称
	bool PsRemoteThreadInjectDll(DWORD dwPid, char* szPath);//远程线程注入dll
	HMODULE PsFindRemoteMod(HANDLE hProcess, const wchar_t * pszModShortName);//获取指定进程中的模块句柄
	bool PsRemoteThreadUnloadDll(DWORD dwPid, wchar_t *dllName);//远程线程卸载dll		  
	bool PsZwCreateThreadExInjectDll(DWORD dwProcessId, char *pszDllFileName);//使用ZwCreateThreadEx实现远线程注入(突破SESSSION 0限制)
	bool PsGetRemoteModulePath(const char* moduleName, short nPID, char* modulepath);//获取指定进程中指定模块的路径(测试一下)
	DWORD PsGetRemoteModuleBaseAddr(const char* moduleName, short nPID);//获取指定进程指定模块的内存地址
	HMODULE PsGetRemoteModuleHandle(const char* moduleName, short nPID);//获取指定进程指定模块的句柄
}
