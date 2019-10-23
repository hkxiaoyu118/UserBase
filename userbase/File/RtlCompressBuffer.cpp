#include "RtlCompressBuffer.h"

void ShowRtlCompressBufferError(char *pszText)
{
	char szErr[MAX_PATH] = { 0 };
	::wsprintfA(szErr, "%s Error[%d]\n", pszText, ::GetLastError());
#ifdef _DEBUG
	::MessageBoxA(NULL, szErr, "ERROR", MB_OK);
#endif
}

// 数据压缩
BOOL CompressData(BYTE *pUncompressData, DWORD dwUncompressDataLength, BYTE **ppCompressData, DWORD *pdwCompressDataLength)
{
	BOOL bRet = FALSE;
	NTSTATUS status = 0;
	HMODULE hModule = NULL;
	typedef_RtlGetCompressionWorkSpaceSize RtlGetCompressionWorkSpaceSize  = NULL;
	typedef_RtlCompressBuffer RtlCompressBuffer = NULL;
	DWORD dwWorkSpaceSize = 0, dwFragmentWorkSpaceSize = 0;
	BYTE *pWorkSpace = NULL;
	BYTE *pCompressData = NULL;
	DWORD dwCompressDataLength = 4096;
	DWORD dwFinalCompressSize = 0;
	do
	{
		// 加载 ntdll.dll 
		hModule = ::LoadLibraryA("ntdll.dll");
		if (NULL == hModule)
		{
			ShowRtlCompressBufferError("LoadLibrary");
			break;
		}

		// 获取 RtlGetCompressionWorkSpaceSize 函数地址
		RtlGetCompressionWorkSpaceSize = (typedef_RtlGetCompressionWorkSpaceSize)::GetProcAddress(hModule, "RtlGetCompressionWorkSpaceSize");
		if (NULL == RtlGetCompressionWorkSpaceSize)
		{
			ShowRtlCompressBufferError("GetProcAddress");
			break;
		}

		// 获取 RtlCompressBuffer 函数地址
		RtlCompressBuffer = (typedef_RtlCompressBuffer)::GetProcAddress(hModule, "RtlCompressBuffer");
		if (NULL == RtlCompressBuffer)
		{
			ShowRtlCompressBufferError("GetProcAddress");
			break;
		}

		// 获取WorkSpqce大小
		status = RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_STANDARD, &dwWorkSpaceSize, &dwFragmentWorkSpaceSize);
		if (0 != status)
		{
			ShowRtlCompressBufferError("RtlGetCompressionWorkSpaceSize");
			break;
		}

		// 申请动态内存
		pWorkSpace = new BYTE[dwWorkSpaceSize];
		if (NULL == pWorkSpace)
		{
			ShowRtlCompressBufferError("new");
			break;
		}
		::RtlZeroMemory(pWorkSpace, dwWorkSpaceSize);
		
		while (TRUE)
		{
			// 申请动态内存
			pCompressData = new BYTE[dwCompressDataLength];
			if (NULL == pCompressData)
			{
				ShowRtlCompressBufferError("new");
				break;
			}
			::RtlZeroMemory(pCompressData, dwCompressDataLength);

			// 调用RtlCompressBuffer压缩数据
			RtlCompressBuffer(COMPRESSION_FORMAT_LZNT1, pUncompressData, dwUncompressDataLength, pCompressData, dwCompressDataLength, 4096, &dwFinalCompressSize, (PVOID)pWorkSpace);
			if (dwCompressDataLength < dwFinalCompressSize)
			{
				// 释放内存
				if (pCompressData)
				{
					delete[]pCompressData;
					pCompressData = NULL;
				}
				dwCompressDataLength = dwFinalCompressSize;
			}
			else
			{
				break;
			}
		}

		// 返回
		*ppCompressData = pCompressData;
		*pdwCompressDataLength = dwFinalCompressSize;
		bRet = TRUE;

	} while(FALSE);

	// 释放
	if (pWorkSpace)
	{
		delete[]pWorkSpace;
		pWorkSpace = NULL;
	}
	if (hModule)
	{
		::FreeLibrary(hModule);
	}

	return bRet;
}


// 数据解压缩
BOOL UncompressData(BYTE *pCompressData, DWORD dwCompressDataLength, BYTE **ppUncompressData, DWORD *pdwUncompressDataLength)
{
	BOOL bRet = FALSE;
	HMODULE hModule = NULL;
	typedef_RtlDecompressBuffer RtlDecompressBuffer = NULL;
	BYTE *pUncompressData = NULL;
	DWORD dwUncompressDataLength = 4096;
	DWORD dwFinalUncompressSize = 0;
	do
	{
		// 加载 ntdll.dll 
		hModule = ::LoadLibraryA("ntdll.dll");
		if (NULL == hModule)
		{
			ShowRtlCompressBufferError("LoadLibrary");
			break;
		}

		// 获取 RtlDecompressBuffer 函数地址
		RtlDecompressBuffer = (typedef_RtlDecompressBuffer)::GetProcAddress(hModule, "RtlDecompressBuffer");
		if (NULL == RtlDecompressBuffer)
		{
			ShowRtlCompressBufferError("GetProcAddress");
			break;
		}

		while (TRUE)
		{
			// 申请动态内存
			pUncompressData = new BYTE[dwUncompressDataLength];
			if (NULL == pUncompressData)
			{
				ShowRtlCompressBufferError("new");
				break;
			}
			::RtlZeroMemory(pUncompressData, dwUncompressDataLength);

			// 调用RtlCompressBuffer压缩数据
			RtlDecompressBuffer(COMPRESSION_FORMAT_LZNT1, pUncompressData, dwUncompressDataLength, pCompressData, dwCompressDataLength, &dwFinalUncompressSize);
			if (dwUncompressDataLength < dwFinalUncompressSize)
			{
				// 释放内存
				if (pUncompressData)
				{
					delete[]pUncompressData;
					pUncompressData = NULL;
				}
				dwUncompressDataLength = dwFinalUncompressSize;
			}
			else
			{
				break;
			}
		}

		// 返回
		*ppUncompressData = pUncompressData;
		*pdwUncompressDataLength = dwFinalUncompressSize;
		bRet = TRUE;

	} while (FALSE);

	// 释放
	if (hModule)
	{
		::FreeLibrary(hModule);
	}

	return bRet;
}
