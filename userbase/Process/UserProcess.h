#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <Psapi.h>
#include <process.h>
#include <TlHelp32.h>

namespace ubase
{
	bool EnableDebugPriv();//提升当前进程权限函数("SeDebugPrivilege"读、写控制权限)
	bool EnableShutdownPriv();//提升当前进程权限函数("SeShutdownPrivilege"关机权限)
	bool EnableBackupPriv();//提升当前进程权限函数("SeBackupPrivilege"备份数据权限)
	bool EnableRestorePriv();//提升当前进程权限函数("SeRestorePrivilege"恢复数据权限)
	bool PsGetPidsByProcessName(std::vector<DWORD> &vDwPids, const std::string &processName);//通过进程名称获取进程pid,进程名不区分大小写
	bool PsGetAllProcessName(std::vector<std::string> &vsProcessName);//获取所有的进程的名称
	bool RemoteThreadInjectDll(DWORD dwPid, char* szPath);//远程线程注入dll
	HMODULE FindRemoteMod(HANDLE hProcess, const wchar_t * pszModShortName);//获取指定进程中的模块句柄
	bool RemoteThreadUnloadDll(DWORD dwPid, wchar_t *dllName);//远程线程卸载dll		  
	bool ZwCreateThreadExInjectDll(DWORD dwProcessId, char *pszDllFileName);//使用ZwCreateThreadEx实现远线程注入(突破SESSSION 0限制)
}
