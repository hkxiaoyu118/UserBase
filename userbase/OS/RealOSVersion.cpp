#include "RealOSVersion.h"
#include <windows.h>

namespace ubase
{
	DWORD SAFE_ADD(const DWORD& a, const DWORD& b, const DWORD& limit = MAXDWORD)
	{
		return ((a >= limit) || (b >= limit) || ((limit - a) <= b)) ? limit : (a + b);
	}

	void InitializeOsVersion(OSVERSIONINFOEXW* const osInfo)
	{
		SecureZeroMemory(osInfo, sizeof(OSVERSIONINFOEXW));
		osInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	}

	DWORD InitializeStepSize(const DWORD& limit)
	{
		DWORD result = 1;
		while (result < limit)
		{
			result = SAFE_ADD(result, result);
		}
		return result;
	}

	bool GetOsInfo(OSVERSIONINFOEXW* const osInfo)
	{
		typedef LONG(__stdcall * RtlGetVersion)(LPOSVERSIONINFOEXW);
		if (const HMODULE ntdll = GetModuleHandleW(L"ntdll"))
		{
			if (const RtlGetVersion pRtlGetVersion = (RtlGetVersion)GetProcAddress(ntdll, "RtlGetVersion"))
			{
				InitializeOsVersion(osInfo);
				if (pRtlGetVersion(osInfo) == 0)
				{
					return true;
				}
			}
		}

		InitializeOsVersion(osInfo);
		return (GetVersionExW((LPOSVERSIONINFOW)osInfo) != FALSE);
	}

	bool VerifyOsInfo(OSVERSIONINFOEXW* const osInfo, const ULONG typeMask, const ULONGLONG condMask)
	{
		typedef LONG(__stdcall * RtlVerifyVersionInfo)(LPOSVERSIONINFOEXW, ULONG, ULONGLONG);
		if (const HMODULE ntdll = GetModuleHandleW(L"ntdll"))
		{
			if (const RtlVerifyVersionInfo pRtlVerifyVersionInfo = (RtlVerifyVersionInfo)GetProcAddress(ntdll, "RtlVerifyVersionInfo"))
			{
				if (pRtlVerifyVersionInfo(osInfo, typeMask, condMask) == 0)
				{
					return true;
				}
			}
		}

		return (VerifyVersionInfoW(osInfo, typeMask, condMask) != FALSE);
	}

	bool VerifyOsVersion(const DWORD major, const DWORD minor)
	{
		OSVERSIONINFOEXW osvi;
		DWORDLONG dwlConditionMask = 0;
		InitializeOsVersion(&osvi);

		osvi.dwMajorVersion = major;
		osvi.dwMinorVersion = minor;
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

		const BOOL ret = VerifyOsInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID, dwlConditionMask);
		return (ret != FALSE);
	}

	bool VerifyOsBuildNo(const DWORD buildNo)
	{
		OSVERSIONINFOEXW osvi;
		DWORDLONG dwlConditionMask = 0;
		InitializeOsVersion(&osvi);

		osvi.dwBuildNumber = buildNo;
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

		VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

		const BOOL ret = VerifyOsInfo(&osvi, VER_BUILDNUMBER, dwlConditionMask);
		return (ret != FALSE);
	}

	bool GetFakeOsVersion(unsigned int* const major, unsigned int* const minor, unsigned int* const build)
	{
		OSVERSIONINFOEXW osInfo;
		InitializeOsVersion(&osInfo);

		if (GetVersionExW((LPOSVERSIONINFOW)& osInfo))
		{
			*major = osInfo.dwMajorVersion;
			*minor = osInfo.dwMinorVersion;
			*build = osInfo.dwBuildNumber;
			return true;
		}

		return false;
	}

	bool GetRealOsVersion(unsigned int* const major, unsigned int* const minor, unsigned int* const build)
	{
		static const DWORD MAX_VERSION = 0xFFFF;
		static const DWORD MAX_BUILDNO = (((DWORD)~((DWORD)0)) >> 1);

		OSVERSIONINFOEXW osvi;

		if (GetOsInfo(&osvi) == FALSE)
		{
			return false;
		}

		if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			*major = osvi.dwMajorVersion;
			*minor = osvi.dwMinorVersion;
			*build = osvi.dwBuildNumber;
		}
		else
		{
			//"Not running on WIN_NT, says get_os_info()"
			if (VerifyOsVersion(4, 0))
			{
				//"Win9x compat mode detected!"
				*major = 4;
				*build = 1381;
			}
			else
			{
				//"Stopping."
				return false;
			}
		}

		//"Initial version: " VERSION_STRING, (*major), (*minor), (*build)

		//Major Version
		for (DWORD nextMajor = (*major) + 1; nextMajor <= MAX_VERSION; nextMajor++)
		{
			if (VerifyOsVersion(nextMajor, 0))
			{
				*major = nextMajor;
				*minor = 0;
				//"--> Bump major version: " VERSION_STRING, (*major), (*minor), (*build)
				continue;
			}
			//"Major version unsupported: %1!u!", nextMajor
			break;
		}

		//Minor Version
		for (DWORD nextMinor = (*minor) + 1; nextMinor <= MAX_VERSION; nextMinor++)
		{
			if (VerifyOsVersion((*major), nextMinor))
			{
				*minor = nextMinor;
				//"--> Bump minor version: " VERSION_STRING, (*major), (*minor), (*build)
				continue;
			}
			//"Minor version unsupported: %1!u!", nextMinor
			break;
		}

		//Build Version
		if (VerifyOsBuildNo(SAFE_ADD((*build), 1, MAX_BUILDNO)))
		{
			DWORD stepSize = InitializeStepSize(MAX_BUILDNO);
			for (DWORD nextBuildNo = SAFE_ADD((*build), stepSize, MAX_BUILDNO); (*build) < MAX_BUILDNO; nextBuildNo = SAFE_ADD((*build), stepSize, MAX_BUILDNO))
			{
				//"Current step size: %1!u!", stepSize
				if (VerifyOsBuildNo(nextBuildNo))
				{
					*build = nextBuildNo;
					//"--> Bump build version: " VERSION_STRING, (*major), (*minor), (*build)
					continue;
				}
				//"Build version unsupported: %1!u!", nextBuildNo
				if (stepSize > 1)
				{
					stepSize = stepSize / 2;
					continue;
				}
				break;
			}
		}
		else
		{
			//Build version unsupported
		}

		if ((*major >= MAX_VERSION) || (*minor >= MAX_VERSION) || (*build >= MAXDWORD))
		{
			//Overflow was detected!
			return false;
		}

		return true;
	}
}
