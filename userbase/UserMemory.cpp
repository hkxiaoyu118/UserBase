#include "stdafx.h"
#include "UserMemory.h"

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

	void WriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
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

	void ReadFromMemory(uintptr_t addressToRead, char* varToWriteTo, int byteNum)
	{
		//memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);
		//memcpy((LPVOID)addressToRead, varToWriteTo, byteNum);
		memcpy((LPVOID)varToWriteTo, (LPVOID)addressToRead, byteNum);
	}

}
