#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <locale.h>
#include <commctrl.h>
#include <atlstr.h>

typedef struct _TrayInfo
{
	RECT rect;				//图标坐标
	std::string title;		//图标显示标题
	std::string filePath;	//图标对应的文件路径
	DWORD pid;				//图标对应进程ID
}TrayInfo;

class TrayWindow
{
public:
	TrayWindow();
	~TrayWindow();
	BOOL IsWow64();															//判断是否是64位操作系统
	HWND FindTrayWnd();														//找到托盘句柄
	HWND FindNotifyIconOverflowWindow();									//找到小托盘句柄(隐藏在小三角下面的托盘图标)
	bool EnumNotifyWindow(HWND hWnd, std::vector<TrayInfo>& trayInfoVec);	//根据窗口句柄,获取该托盘面板所有的托盘图标的信息
};
