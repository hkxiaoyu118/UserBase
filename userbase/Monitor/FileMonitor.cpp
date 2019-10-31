#include "FileMonitor.h"

void W2C(wchar_t *pwszSrc, int iSrcLen, char *pszDest, int iDestLen)
{
	::RtlZeroMemory(pszDest, iDestLen);
	::WideCharToMultiByte(CP_ACP,
		0,
		pwszSrc,
		(iSrcLen / 2),
		pszDest,
		iDestLen,
		NULL,
		NULL);
}

void MonitorFileThread(LPVOID args)
{
	char* pszDirectory = (char*)args; //目录结尾必须以'\'结尾,否则不起作用

	//打开目录获取文件句柄
	HANDLE hDirectory = CreateFileA(pszDirectory, FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (hDirectory == INVALID_HANDLE_VALUE)
	{
#ifdef _DEBUG
		MessageBoxA(NULL, "CreateFile Failed", "错误", MB_OK);
#endif
		return;
	}

	char szTemp[MAX_PATH] = { 0 };
	BOOL bRet = FALSE;
	DWORD dwRet = 0;
	DWORD dwBufferSize = 4096;

	BYTE* pBuf = new BYTE[dwBufferSize];
	if (pBuf == NULL)
	{
#ifdef _DEBUG
		MessageBoxA(NULL, "malloc Failed", "错误", MB_OK);
#endif
		return;
	}

	FILE_NOTIFY_INFORMATION *pFileNotifyInfo = (FILE_NOTIFY_INFORMATION *)pBuf;

	// 开始循环设置监控
	do
	{
		::RtlZeroMemory(pFileNotifyInfo, dwBufferSize);
		// 设置监控目录
		bRet = ::ReadDirectoryChangesW(hDirectory,
			pFileNotifyInfo,
			dwBufferSize,
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |			// 修改文件名
			FILE_NOTIFY_CHANGE_ATTRIBUTES |			// 修改文件属性
			FILE_NOTIFY_CHANGE_LAST_WRITE,			// 最后一次写入
			&dwRet,
			NULL,
			NULL);
		if (FALSE == bRet)
		{
#ifdef _DEBUG
			MessageBoxA(NULL, "ReadDirectoryChangesW Failed", "错误", MB_OK);
#endif
			break;
		}
		//将宽字符转换成窄字符
		W2C((wchar_t *)(&pFileNotifyInfo->FileName), pFileNotifyInfo->FileNameLength, szTemp, MAX_PATH);
		//判断文件的操作类型(还有很多类型,可以根据需要进行添加)
		switch (pFileNotifyInfo->Action)
		{
		case FILE_ACTION_ADDED:
		{
			//新增文件
			break;
		}
		default:
		{
			break;
		}
		}


	} while (bRet);
	//关闭句柄, 释放内存
	::CloseHandle(hDirectory);
	delete[] pBuf;
	pBuf = NULL;
}