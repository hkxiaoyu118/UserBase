// userbase_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../userbase/ubase.h"

#pragma comment(lib, "../Release/Bin/userbase.lib")

#define _CRT_SECURE_NO_WARNINGS

int main()
{
	DWORD pid = GetCurrentProcessId();
	__int64 startTime = ubase::TimerGetProcessCreateTime(pid);
	printf("进程的启动时间是:%lld", startTime);
}
