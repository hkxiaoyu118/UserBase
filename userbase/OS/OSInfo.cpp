#include "OSInfo.h"
#include <windows.h>

namespace ubase
{
	OSInfo::OSInfo()
	{

	}

	OSInfo::~OSInfo()
	{

	}

	WIN_OS_VERSION OSInfo::GetOSVersion()
	{
		OSVERSIONINFO Version;
		ZeroMemory(&Version, sizeof(OSVERSIONINFO));
		Version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&Version);
		if (Version.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((Version.dwMajorVersion == 5) && (Version.dwMinorVersion == 0))
			{
				return WIN_2000;//Win2000;
			}
			else if ((Version.dwMajorVersion == 5) && (Version.dwMinorVersion == 1))
			{
				return WIN_XP; //WinXP;
			}
			else if ((Version.dwMajorVersion == 5) && (Version.dwMinorVersion == 2))
			{
				return WIN_2003; //Win2003;
			}
			else if ((Version.dwMajorVersion == 6) && (Version.dwMinorVersion == 0))
			{
				return WIN_VISTA; //VISTA;
			}
			else if ((Version.dwMajorVersion >= 6))
			{
				BOOL bIsWow64 = FALSE;

				typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
				LPFN_ISWOW64PROCESS pfnIsWow64 = NULL;

				pfnIsWow64 = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleA("kernel32.dll"), "IsWow64Process");
				if (pfnIsWow64)
				{
					pfnIsWow64(GetCurrentProcess(), &bIsWow64);
				}

				if (Version.dwMinorVersion == 2 && bIsWow64 == false)   //WIN8
					return WIN_8x32;
				else if (Version.dwMinorVersion == 2 && bIsWow64 == true)
				{
					return WIN_8x64; //WIN8x64
				}

				if (bIsWow64)
					return WIN_7x64;//win7x64
				return WIN_7x32;   //win7x64
			}
		}
		return WIN_UNKNOWN;
	}
}