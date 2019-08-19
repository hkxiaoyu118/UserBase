#include "../stdafx.h"
#include "UserProcess.h"
#include "../comdef.h"

#pragma comment(lib, "Psapi.lib")

namespace ubase
{
	bool PsEnablePrivilege(LPCSTR pszPrivName, bool fEnable)
	{
		bool bResult = false;  
		HANDLE hToken;
 
		if (OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			TOKEN_PRIVILEGES tp = { 1 };

			if (LookupPrivilegeValueA(NULL, pszPrivName, &tp.Privileges[0].Luid))
			{
				tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;

				AdjustTokenPrivileges(hToken, false, &tp, sizeof(tp), NULL, NULL);

				bResult = (GetLastError() == ERROR_SUCCESS);
			}
			CloseHandle(hToken);
		}
		return bResult;
	}

	bool PsEnableDebugPriv()
	{
		HANDLE hToken;
		LUID seDebugNameValue;
		TOKEN_PRIVILEGES tkp;

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return false;

		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &seDebugNameValue))
		{
			CloseHandle(hToken);
			return false;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = seDebugNameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
			CloseHandle(hToken);

		return true;
	}

	bool PsEnableShutdownPriv()
	{
		HANDLE hToken;
		LUID seDebugNameValue;
		TOKEN_PRIVILEGES tkp;

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return false;

		if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &seDebugNameValue))
		{
			CloseHandle(hToken);
			return false;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = seDebugNameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
			CloseHandle(hToken);

		return true;
	}

	bool PsEnableBackupPriv()
	{
		HANDLE hToken;
		LUID seDebugNameValue;
		TOKEN_PRIVILEGES tkp;

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return false;

		if (!LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &seDebugNameValue))
		{
			CloseHandle(hToken);
			return false;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = seDebugNameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
			CloseHandle(hToken);

		return true;
	}

	bool PsEnableRestorePriv()
	{
		HANDLE hToken;
		LUID seDebugNameValue;
		TOKEN_PRIVILEGES tkp;

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return false;

		if (!LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &seDebugNameValue))
		{
			CloseHandle(hToken);
			return false;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = seDebugNameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
			CloseHandle(hToken);

		return true;
	}

	bool PsGetPidsByProcessName(std::vector<DWORD> &vDwPids, const std::string &processName)
	{
		vDwPids.clear();
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
		{
			return false;
		}

		PROCESSENTRY32 pe = { sizeof(pe) };
		for (BOOL fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		{
			if (!_strcmpi(pe.szExeFile, processName.c_str()))
			{
				DWORD pid = pe.th32ProcessID;
				vDwPids.push_back(pid);
			}
		}
		CloseHandle(hSnapshot);

		if (vDwPids.size() != 0)
			return true;

		return false;
	}

	bool PsGetAllProcessName(std::vector<std::string> &vsProcessName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
		{
			return false;
		}

		PROCESSENTRY32 pe = { sizeof(pe) };
		for (BOOL bOk = Process32First(hSnapshot, &pe); bOk; bOk = Process32Next(hSnapshot, &pe))
		{
			vsProcessName.push_back(pe.szExeFile);
		}
		CloseHandle(hSnapshot);
		if (vsProcessName.size() != 0)
			return true;
		return false;
	}

	bool PsRemoteThreadInjectDll(DWORD dwPid, char* szPath)
	{
		bool bResult = false;
		//1 在目标进程中申请一个空间
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess != NULL)
		{
			LPVOID pRemoteAddress = VirtualAllocEx(
				hProcess,
				NULL,
				1,
				MEM_COMMIT,
				PAGE_READWRITE
			);
			if (pRemoteAddress != NULL)
			{
				//2 把dll的路径写入到对方的进程空间中
				DWORD dwWriteSize = 0;
				//写一段数据到指定进程所开辟的内存空间
				WriteProcessMemory(hProcess, pRemoteAddress, szPath, strlen(szPath) + 1, &dwWriteSize);

				//3 创建一个远程线程，让目标进程调用LoadLibrary
				HANDLE hThread = CreateRemoteThread(
					hProcess,
					NULL,
					0,
					(LPTHREAD_START_ROUTINE)LoadLibraryA,
					pRemoteAddress,
					NULL,
					NULL
				);
				if (hThread != NULL)
				{
					bResult = true;
				}
			}
		}
		return bResult;
	}

	HMODULE PsFindRemoteMod(HANDLE hProcess, const wchar_t * pszModShortName)
	{
		HMODULE hModules[0x100] = { 0 };
		int cbNeeded = 0;
		if (!EnumProcessModules(hProcess
			, hModules
			, sizeof(hModules)
			, (unsigned long *)&cbNeeded))
			return (HMODULE)0;

		for (int i = 0; i < (cbNeeded >> 2); i++)
		{
			wchar_t szModuleName[0x200] = { 0 };
			GetModuleBaseNameW(hProcess
				, hModules[i]
				, szModuleName
				, sizeof(szModuleName) / sizeof(szModuleName[0]));

			if (!_wcsicmp(szModuleName, pszModShortName))
				return hModules[i];
		}
		return (HMODULE)0;
	}

	bool PsRemoteThreadUnloadDll(DWORD dwPid, wchar_t *dllName)
	{
		bool bResult = false;
		// 打开进程，具备写入权限和创建线程权限 
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ
			| PROCESS_VM_WRITE
			| PROCESS_VM_OPERATION
			| PROCESS_SUSPEND_RESUME
			| PROCESS_CREATE_THREAD
			| PROCESS_QUERY_INFORMATION
			, FALSE
			, dwPid);
		if (NULL == hProcess)
			return bResult;

		// 检查是否是同一个用户会话中的进程
		HANDLE hToken = INVALID_HANDLE_VALUE;
		if (!OpenProcessToken(hProcess, TOKEN_READ, &hToken))
		{
			CloseHandle(hProcess);
			return bResult;
		}
		CloseHandle(hToken);

		HMODULE hDllModule = PsFindRemoteMod(hProcess, dllName);

		if (hDllModule != 0)
		{
			// 以FreeLibrary为线程体创建线程，卸载目标进程中的特定DLL. 
			unsigned long tid = 0;
			CreateRemoteThread(hProcess
				, 0, 0
				, (LPTHREAD_START_ROUTINE)FreeLibrary
				, hDllModule
				, 0
				, &tid);
			bResult = true;
		}
		CloseHandle(hProcess);
		hProcess = 0;
		return bResult;
	}

	bool PsZwCreateThreadExInjectDll(DWORD dwProcessId, char *pszDllFileName)
	{
		HANDLE hProcess = NULL;
		SIZE_T dwSize = 0;
		LPVOID pDllAddr = NULL;
		FARPROC pFuncProcAddr = NULL;
		HANDLE hRemoteThread = NULL;
		DWORD dwStatus = 0;

		// 打开注入进程，获取进程句柄
		hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if (NULL == hProcess)
		{
			return false;
		}
		// 在注入进程中申请内存
		dwSize = 1 + ::lstrlen(pszDllFileName);
		pDllAddr = ::VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (NULL == pDllAddr)
		{
			return false;
		}
		// 向申请的内存中写入数据
		if (FALSE == ::WriteProcessMemory(hProcess, pDllAddr, pszDllFileName, dwSize, NULL))
		{
			return false;
		}
		// 加载 ntdll.dll
		HMODULE hNtdllDll = ::LoadLibrary("ntdll.dll");
		if (NULL == hNtdllDll)
		{
			return false;
		}
		// 获取LoadLibraryA函数地址
		pFuncProcAddr = ::GetProcAddress(::GetModuleHandle("Kernel32.dll"), "LoadLibraryA");
		if (NULL == pFuncProcAddr)
		{
			return false;
		}
		// 获取ZwCreateThread函数地址
#ifdef _WIN64
		typedef DWORD(WINAPI *typedef_ZwCreateThreadEx)(
			PHANDLE ThreadHandle,
			ACCESS_MASK DesiredAccess,
			LPVOID ObjectAttributes,
			HANDLE ProcessHandle,
			LPTHREAD_START_ROUTINE lpStartAddress,
			LPVOID lpParameter,
			ULONG CreateThreadFlags,
			SIZE_T ZeroBits,
			SIZE_T StackSize,
			SIZE_T MaximumStackSize,
			LPVOID pUnkown);
#else
		typedef DWORD(WINAPI *typedef_ZwCreateThreadEx)(
			PHANDLE ThreadHandle,
			ACCESS_MASK DesiredAccess,
			LPVOID ObjectAttributes,
			HANDLE ProcessHandle,
			LPTHREAD_START_ROUTINE lpStartAddress,
			LPVOID lpParameter,
			BOOL CreateSuspended,
			DWORD dwStackSize,
			DWORD dw1,
			DWORD dw2,
			LPVOID pUnkown);
#endif
		typedef_ZwCreateThreadEx ZwCreateThreadEx = (typedef_ZwCreateThreadEx)::GetProcAddress(hNtdllDll, "ZwCreateThreadEx");
		if (NULL == ZwCreateThreadEx)
		{
			return false;
		}
		// 使用 ZwCreateThreadEx 创建远线程, 实现 DLL 注入
		dwStatus = ZwCreateThreadEx(&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)pFuncProcAddr, pDllAddr, 0, 0, 0, 0, NULL);
		if (NULL == hRemoteThread)
		{
			return false;
		}
		// 关闭句柄
		::CloseHandle(hProcess);
		::FreeLibrary(hNtdllDll);

		return true;
	}

	bool PsGetRemoteModulePath(const char* moduleName, short nPID, char* modulepath)
	{
		HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPID);
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32);

		Module32First(tlh, &modEntry);
		do
		{
			if (!_stricmp(moduleName, modEntry.szModule))
			{
				CloseHandle(tlh);
				strcpy(modulepath, modEntry.szExePath);
				return true;
			}
		} while (Module32Next(tlh, &modEntry));
		CloseHandle(tlh);

		return false;
	}

	DWORD PsGetRemoteModuleBaseAddr(const char* moduleName, short nPID)
	{
		HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPID);
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32);

		Module32First(tlh, &modEntry);
		do
		{
			if (!_stricmp(moduleName, modEntry.szModule))
			{
				CloseHandle(tlh);
				return (DWORD)modEntry.modBaseAddr;
			}
		} while (Module32Next(tlh, &modEntry));
		CloseHandle(tlh);

		return NULL;
	}

	HMODULE PsGetRemoteModuleHandle(const char* moduleName, short nPID)
	{
		HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPID);
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32);

		Module32First(tlh, &modEntry);
		do
		{
			if (!_stricmp(moduleName, modEntry.szModule))
			{
				CloseHandle(tlh);
				return modEntry.hModule;
			}
		} while (Module32Next(tlh, &modEntry));
		CloseHandle(tlh);

		return NULL;
	}

	std::string PsGetProcesFullPath(HANDLE hProcess)
	{
		DWORD dwRet = 0;
		CHAR szImageName[MAX_PATH] = { 0 };
		std::string result;
		dwRet = GetProcessImageFileName(hProcess, szImageName, MAX_PATH);//获取进程的DOS文件路径
		if (dwRet > 0)
		{
			result = PsDevicePathToDosPath(szImageName);//设备名称转DOS名称
		}
		return result;
	}

	std::string PsDevicePathToDosPath(std::string devPath)
	{
		CHAR szDriveStr[512];
		CHAR szDrive[3];
		CHAR szDevName[64];
		std::string result;
		
		if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
		{
			for (int i = 0; szDriveStr[i]; i += 4)
			{
				szDrive[0] = szDriveStr[i];
				szDrive[1] = szDriveStr[i + 1];
				szDrive[2] = '\0';
				if (!QueryDosDevice(szDrive, szDevName, 64))
					break;

				int length = lstrlen(szDevName);
				if (_strnicmp(devPath.c_str(), szDevName, length) == 0)
				{
					result = szDrive;
					result = result + devPath.substr(length);
					break;
				}
			}
		}

		return result;
	}

	std::string PsDosPathToDevicePath(std::string dosPath)
	{
		CHAR szDevName[64];
		std::string result;
		std::string dos = dosPath.substr(0, 2);
		if (QueryDosDevice(dos.c_str(), szDevName, 64))
		{
			result = szDevName;
			result = result + dosPath.substr(2);
		}
		return result;
	}

	bool PsCreateProcess(std::string pePath)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcess(
			NULL,						//指向一个NULL结尾的、用来指定可执行模块的宽字节字符串  
			(LPSTR)pePath.c_str(),		//命令行字符串  
			NULL,						//指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。  
			NULL,						//如果lpProcessAttributes参数为空（NULL）,那么句柄不能被继承。<同上>  
			false,						//指示新进程是否从调用进程处继承了句柄。   
			0,							//指定附加的、用来控制优先类和进程的创建的标  
										//CREATE_NEW_CONSOLE  新控制台打开子进程  
										//CREATE_SUSPENDED 子进程创建后挂起,直到调用ResumeThread函数  
			NULL,						//指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境  
			NULL,						//指定子进程的工作路径  
			&si,						//决定新进程的主窗体如何显示的STARTUPINFO结构体  
			&pi							//接收新进程的识别信息的PROCESS_INFORMATION结构体  
		))
		{
			return true;
		}

		return false;
	}

	bool PsCreateProcessReturnInfo(std::string pePath, PROCESS_INFORMATION& pi)
	{
		STARTUPINFO si;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcess(
			NULL,						//指向一个NULL结尾的、用来指定可执行模块的宽字节字符串  
			(LPSTR)pePath.c_str(),		//命令行字符串  
			NULL,						//指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。  
			NULL,						//如果lpProcessAttributes参数为空（NULL）,那么句柄不能被继承。<同上>  
			false,						//指示新进程是否从调用进程处继承了句柄。   
			0,							//指定附加的、用来控制优先类和进程的创建的标  
										//CREATE_NEW_CONSOLE  新控制台打开子进程  
										//CREATE_SUSPENDED 子进程创建后挂起,直到调用ResumeThread函数  
			NULL,						//指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境  
			NULL,						//指定子进程的工作路径  
			&si,						//决定新进程的主窗体如何显示的STARTUPINFO结构体  
			&pi							//接收新进程的识别信息的PROCESS_INFORMATION结构体  
		))
		{
			return true;
		}

		return false;
	}
}


