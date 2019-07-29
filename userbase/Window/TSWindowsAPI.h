#pragma once
#include <windows.h>

namespace ubase
{
	class TSWindowsAPI
	{
	public:
		TSWindowsAPI(void);
		~TSWindowsAPI(void);

	public:
		int m_retStringLen;
		DWORD m_isEuemprosuccess;
		DWORD m_nPid[MAX_PATH];
		bool TSEnumWindow(HWND parent, char* title, char* class_name, LONG filter, char* retstring, char* process_name = NULL);
		bool TSEnumWindowSuper(char* spec1, LONG flag1, LONG type1, char* spec2, LONG flag2, LONG type2, LONG sort, char* retstring = NULL);
		bool TSEnumProcess(char* name, char* retstring);
		bool TSClientToScreen(LONG hwnd, LONG& x, LONG& y);
		bool TSFindWindow(char* class_name, char* title, LONG& rethwnd, DWORD parent = 0);
		bool TSFindWindowByProcess(char* class_name, char* title, LONG& rethwnd, char* process_name = NULL, DWORD Pid = 0);
		bool TSGetClientRect(LONG hwnd, LONG& x, LONG& y, LONG& x1, LONG& y1);
		bool TSGetClientSize(LONG hwnd, LONG& width, LONG& height);
		bool TSGetMousePointWindow(LONG& rethwnd, LONG x = -1, LONG y = -1);
		bool TSGetProcessInfo(LONG pid, char* retstring);
		bool TSGetWindow(LONG hwnd, LONG flag, LONG& rethwnd);
		bool GetProcesspath(DWORD ProcessID, char* process_path);
		bool TSGetWindowState(LONG hwnd, LONG flag);
		bool TSSendPaste(LONG hwnd);
		bool TSSetWindowSize(LONG hwnd, LONG width, LONG hight, int type = 0);
		bool TSSetWindowState(LONG hwnd, LONG flag, LONG rethwnd = 0);
		bool TSSetWindowTransparent(LONG hwnd, LONG trans);
		bool TSSetClipboard(wchar_t* values);
		bool TSGetClipboard(wchar_t* retstr);
	private:
		DWORD  FindChildWnd(HWND hchile, char* title, char* classname, char* retstring, bool isGW_OWNER = false, bool isVisible = false, char* process_name = NULL);
		BOOL   EnumProcessbyName(DWORD   dwPID, LPCSTR   ExeName, LONG type = 0);
		int GetProcessNumber();//获取CPU个数
		// 时间格式转换
		__int64 FileTimeToInt64(const FILETIME& time);
		double GetCpuUsage(DWORD ProcessID);//获取指定进程CPU使用率
		DWORD GetMemoryInfo(DWORD ProcessID);//或者指定进程内存使用率
	};
}
