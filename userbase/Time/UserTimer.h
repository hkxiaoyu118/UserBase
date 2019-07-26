#pragma once
#include <windows.h>

namespace ubase
{
	__int64 TimerGetUnixTime();//获取当前的UNIX时间戳
	__int64 TimerGetProcessCreateTime(DWORD dwPid);//获取进程的启动时间
}