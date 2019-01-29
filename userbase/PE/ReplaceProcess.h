#ifndef _REPLACE_PROCESS_H_
#define _REPLACE_PROCESS_H_


#include <Windows.h>


// 创建进程并替换进程内存数据, 更改执行顺序
// 可以用于创建傀儡进程
BOOL ReplaceProcess(char *pszFilePath, PVOID pReplaceData, DWORD dwReplaceDataSize, DWORD dwRunOffset);


#endif