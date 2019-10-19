#include "AutoRun.h"

/*
	* 创建快捷方式
	* szExePath[in]:要创建快捷方式的exe文件全路径
	* szLinkName[in]:要创建的快捷方式.link文件的全路径
	* iIcon[in]:要创建快捷方式的exe文件RC资源中的icon值（默认为0）
*/
BOOL PsCreateOneLink(LPCTSTR  lpszExePath, LPCTSTR lpszLnkPath, int iIcon)
{
	if (NULL == lpszExePath)
	{
		return ERROR_INVALID_PARAMETER;
	}

	DWORD dwRlt = ERROR_SUCCESS;
	HRESULT hres = S_OK;
	IShellLink* pShellLink = NULL;
	IPersistFile* pPersistFile = NULL;
	WCHAR szwShortCutName[MAX_PATH];
	try
	{
		::CoInitialize(NULL); // 初始化 COM 库
		// 创建 COM 对象并获取其实现的接口
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
		if (FAILED(hres))
		{
			throw(hres);
		}

		TCHAR szWorkPath[MAX_PATH] = { 0 };
		StringCchCopy(szWorkPath, MAX_PATH, lpszExePath);
		LPTSTR pszEnd = _tcsrchr(szWorkPath, _T('\\'));
		if (pszEnd != NULL)
		{
			*pszEnd = NULL;
		}

		// 设置快捷方式的各种属性
		pShellLink->SetPath(lpszExePath); // 快捷方式所指的应用程序名
		pShellLink->SetDescription(_T("描述")); // 描述
		pShellLink->SetWorkingDirectory(szWorkPath); // 设置工作目录
		pShellLink->SetIconLocation(lpszExePath, iIcon);//直接取exe文件中的图标进行设置

		// 查询 IShellLink 接口从而得到 IPersistFile 接口来保存快捷方式
		hres = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
		if (FAILED(hres))
		{
			throw(hres);
		}
		StringCchPrintfW(szwShortCutName, MAX_PATH, L"%s", lpszLnkPath);
		//使用 IPersistFile 接口的 Save() 方法保存快捷方式
		hres = pPersistFile->Save(szwShortCutName, TRUE);
	}
	catch (DWORD dwExpRlt)
	{
		dwRlt = dwExpRlt;
	}
	catch (...)
	{
	}

	if (pPersistFile != NULL) {
		pPersistFile->Release(); pPersistFile = NULL;
	}

	if (pShellLink != NULL) {
		pShellLink->Release(); pPersistFile = NULL;
	}
	CoUninitialize();
	return dwRlt;
}

/*
	* 设置桌面快捷方式
	* szExePath[in]:要创建快捷方式的exe文件全路径
	* szLinkName[in]:要创建的快捷方式的名字
	* iIcon[in]:要创建快捷方式的exe文件RC资源中的icon值（默认为0）
*/
void PsCreateDesktopLink(LPCTSTR szExePath, LPCTSTR szLinkName, int iIcon)
{
	LPITEMIDLIST pidlBeginAt, pidlDestination;
	TCHAR szDesktopPath[MAX_PATH] = { 0 };
	LPITEMIDLIST  ppidl = NULL;

	//获取所有用户存放桌面快捷方式的路径
	HRESULT hSpecialPath = SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, &ppidl);
	if (hSpecialPath == S_OK)
	{
		BOOL flag = SHGetPathFromIDList(ppidl, szDesktopPath);
		CoTaskMemFree(ppidl);
	}
	else
	{
		//失败
		return;
	}

	//设置快捷方式.link文件的全路径
	TCHAR szExeLinkFullPath[MAX_PATH] = { 0 };
	StringCchPrintf(szExeLinkFullPath, _countof(szExeLinkFullPath), _T("%s\\%s.lnk"), szDesktopPath, szLinkName);
	//开始创建快捷方式
	PsCreateOneLink(szExePath, szExeLinkFullPath, iIcon);
	// 通知shell有关变化
	SHChangeNotify(SHCNE_CREATE | SHCNE_INTERRUPT, SHCNF_FLUSH | SHCNF_PATH, szExeLinkFullPath, 0);
}