#include "UserTimer.h"
#include <time.h>

namespace ubase
{
	__int64 TimerGetUnixTime()
	{
		time_t t = time(0);
		return t;
	}

	__int64 TimerGetProcessCreateTime(DWORD dwPid)
	{
		LONGLONG llCreateTime = 0;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
		if (hProcess != NULL)
		{
			FILETIME ftCreation, ftExit, ftKernel, ftUser;
			if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser) == TRUE)
			{
				ULARGE_INTEGER ui;
				ui.LowPart = ftCreation.dwLowDateTime;
				ui.HighPart = ftCreation.dwHighDateTime;
				llCreateTime = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
			}
			CloseHandle(hProcess);
		}
		return llCreateTime;
	}
}
