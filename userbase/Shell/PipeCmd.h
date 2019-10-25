#pragma once
#include <Windows.h>

BOOL PipeCmd(char *pszCmd, char *pszResultBuffer, DWORD dwResultBufferSize); //执行cmd命令,并获取执行结果数据(本地使用)
