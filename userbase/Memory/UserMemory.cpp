#include "../stdafx.h"
#include "UserMemory.h"
#include "../comdef.h"

#pragma comment(lib, "Psapi.lib")

namespace ubase
{
	MODULEINFO MmGetModuleInfo(char *szModule)
	{
		MODULEINFO modinfo = { 0 };
		HMODULE hModule = GetModuleHandle(szModule);
		if (hModule == 0)
			return modinfo;
		GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
		return modinfo;
	}

	DWORD MmFindPattern(char *module, char *pattern, char *mask)
	{
		//Get all module related information
		MODULEINFO mInfo = MmGetModuleInfo(module);

		//Assign our base and module size
		//Having the values right is ESSENTIAL, this makes sure
		//that we don't scan unwanted memory and leading our game to crash
		DWORD base = (DWORD)mInfo.lpBaseOfDll;
		DWORD size = (DWORD)mInfo.SizeOfImage;

		//Get length for our mask, this will allow us to loop through our array
		DWORD patternLength = (DWORD)strlen(mask);

		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++)
			{
				//if we have a ? in our mask then we have true by default, 
				//or if the bytes match then we keep searching until finding it or not
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}

			//found = true, our entire pattern was found
			//return the memory addy so we can write to it
			if (found)
			{
				return base + i;
			}
		}
		return NULL;
	}

	void MmWriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
	{
		//used to change our file access type, stores the old
		//access type and restores it after memory is written
		unsigned long OldProtection;
		//give that address read and write permissions and store the old permissions at oldProtection
		VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);

		//write the memory into the program and overwrite previous value
		memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

		//reset the permissions of the address back to oldProtection after writting memory
		VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
	}

	void MmReadFromMemory(uintptr_t addressToRead, char* varToWriteTo, int byteNum)
	{
		//memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);
		//memcpy((LPVOID)addressToRead, varToWriteTo, byteNum);
		memcpy((LPVOID)varToWriteTo, (LPVOID)addressToRead, byteNum);
	}

	ULONG_PTR MmFindImageBase(HANDLE hProc, LPSTR lpCommandLine)
	{
		ULONG_PTR uResult = 0;
		CHAR szBuf[1024] = { 0 };
		SIZE_T dwSize = 0;
		PBYTE pAddress = NULL;

		MEMORY_BASIC_INFORMATION mbi = { 0 };
		BOOL bFoundMemImage = FALSE;
		char szImageFilePath[MAX_PATH] = { 0 };
		char* pFileNameToCheck = strrchr(lpCommandLine, '\\');

		//获取页的大小
		SYSTEM_INFO sysinfo;
		ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO));
		GetSystemInfo(&sysinfo);

		//查找第一个具有MEM_IMAGE属性的页
		pAddress = (PBYTE)sysinfo.lpMinimumApplicationAddress;
		while (pAddress < (PBYTE)sysinfo.lpMaximumApplicationAddress)
		{
			ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
			dwSize = VirtualQueryEx(hProc, pAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
			if (dwSize == 0)
			{
				pAddress += sysinfo.dwPageSize;
				continue;
			}

			switch (mbi.State)
			{
			case MEM_FREE:
			case MEM_RESERVE:
				pAddress = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
				break;
			case MEM_COMMIT:
				if (mbi.Type == MEM_IMAGE)
				{
					if (GetMappedFileName(hProc, pAddress, szImageFilePath, MAX_PATH) != 0)
					{
						char* pCompare = strrchr(szImageFilePath, '\\');
						if (_stricmp(pCompare, pFileNameToCheck) == 0)
						{
							bFoundMemImage = TRUE;
							uResult = (ULONG_PTR)pAddress;
							break;
						}
					}
				}
				pAddress = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
				break;
			default:
				break;
			}

			if (bFoundMemImage == TRUE)
			{
				break;
			}
		}
		return uResult;
	}

	ULONG_PTR MmFindExeImageBase(HANDLE hProc)
	{
		ULONG_PTR uResult = 0;
		HMODULE hModule = ::LoadLibraryA("ntdll.dll");
		if (hModule != NULL)
		{
			pZwQueryInformationProcess ZwQueryInformationProcess = (pZwQueryInformationProcess)GetProcAddress(hModule, "ZwQueryInformationProcess");
			if (ZwQueryInformationProcess != NULL)
			{
				PROCESS_BASIC_INFORMATION pbi;
				ULONG ret = 0;
				ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
				NTSTATUS ntResult = ZwQueryInformationProcess(hProc, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), &ret);
				if (ntResult == ERROR_SUCCESS)
				{
					PEB peb;
					SIZE_T rt = 0;
					BOOL rdResult = ::ReadProcessMemory(hProc, pbi.PebBaseAddress, (LPVOID)& peb, sizeof(PEB), &rt);//读取目标进程中的PEB
					if (rdResult == TRUE)
					{
						uResult = (ULONG_PTR)(peb.Reserved3[1]);//PEB的偏移0x8就是ImagePath
					}
				}
			}
		}
		return uResult;
	}
}
