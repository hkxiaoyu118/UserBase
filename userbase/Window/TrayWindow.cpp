#include "TrayWindow.h"

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

TrayWindow::TrayWindow() {}
TrayWindow::~TrayWindow() {}

BOOL TrayWindow::IsWow64()
{
	BOOL bIsWow64 = FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
}

HWND TrayWindow::FindTrayWnd()
{
	HWND hWnd = NULL;
	hWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("TrayNotifyWnd"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("SysPager"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);
	return hWnd;
}

HWND TrayWindow::FindNotifyIconOverflowWindow()
{
	HWND hWnd = NULL;
	hWnd = FindWindow(_T("NotifyIconOverflowWindow"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);
	return hWnd;
}

bool TrayWindow::EnumNotifyWindow(HWND hWnd, std::vector<TrayInfo>& trayInfoVec)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		return false;
	}

	LPVOID pBun = VirtualAllocEx(hProcess, 0, sizeof(RECT), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	LPVOID lAddress = VirtualAllocEx(hProcess, 0, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lAddress == NULL)
	{
		return false;
	}

	DWORD lTextAdr = 0;
	BYTE buff[1024] = { 0 };
	TCHAR ptb[256] = { 0 };
	CString strFilePath;
	CString strTile;
	HWND hMainWnd = NULL;
	RECT rect;

	unsigned int nDataOffset = sizeof(TBBUTTON) - sizeof(INT_PTR) - sizeof(DWORD_PTR);
	unsigned int nStrOffset = 18;

	//判断是否是64位系统
	if (IsWow64() == TRUE)
	{
		nDataOffset += 4;
		nStrOffset += 6;
	}
	GetWindowRect(hWnd, &rect);//获取窗口的坐标

	//获取图标个数
	int lButton = SendMessage(hWnd, TB_BUTTONCOUNT, 0, 0);
	for (int i = 0; i < lButton; i++)
	{
		DWORD processId = 0;
		SendMessage(hWnd, TB_GETITEMRECT, i, (LPARAM)pBun);
		SendMessage(hWnd, TB_GETBUTTON, i, (LPARAM)lAddress);
		ReadProcessMemory(hProcess, pBun, (LPVOID)ptb, sizeof(RECT), 0);//读坐标
		ReadProcessMemory(hProcess, (LPVOID)((DWORD)lAddress + nDataOffset), &lTextAdr, 4, 0);//读文本地址
		if (lTextAdr != -1)
		{
			ReadProcessMemory(hProcess, (LPCVOID)lTextAdr, buff, 1024, 0);//读文本内容
			hMainWnd = (HWND)(*((DWORD*)buff));
			strFilePath = (WCHAR*)buff + nStrOffset;
			strTile = (WCHAR*)buff + nStrOffset + MAX_PATH;
			::GetWindowThreadProcessId(hMainWnd, &processId);
		}

		//坐标转换
		LPRECT pRect = (LPRECT)ptb;
		OffsetRect(pRect, rect.left, rect.top);

		//将托盘图标信息添加到结果中
		TrayInfo trayInfo;
		trayInfo.rect = *pRect;
		trayInfo.title = strTile.GetBuffer();
		trayInfo.filePath = strFilePath.GetBuffer();
		trayInfo.pid = processId;
		trayInfoVec.push_back(trayInfo);
	}

	VirtualFreeEx(hProcess, pBun, 4096, MEM_DECOMMIT);
	VirtualFreeEx(hProcess, lAddress, 4096, MEM_RELEASE);
	CloseHandle(hProcess);

	return true;
}
