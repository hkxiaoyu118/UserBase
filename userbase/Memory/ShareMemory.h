#pragma once
#include "../stdafx.h"
#include <windows.h>

namespace ubase
{
	class ShareMemory
	{
	public:
		// 构造函数和析构函数
		ShareMemory(const char* pszMapName, int nFileSize = 0, BOOL bServer = FALSE);
		~ShareMemory();
		// 属性
		LPVOID GetBuffer() const { return  m_pBuffer; }
		HANDLE	m_hFileMap;
		// 实现
	private:
		LPVOID	m_pBuffer;
	};

	ShareMemory::ShareMemory(const char* pszMapName,
		int nFileSize, BOOL bServer) : m_hFileMap(NULL), m_pBuffer(NULL)
	{
		if (bServer)
		{
			// 创建一个内存映射文件对象
			m_hFileMap = CreateFileMappingA(INVALID_HANDLE_VALUE,
				NULL, PAGE_READWRITE, 0, nFileSize, pszMapName);
		}
		else
		{
			// 打开一个内存映射文件对象
			m_hFileMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, pszMapName);
		}

		// 映射它到内存，取得共享内存的首地址
		m_pBuffer = (LPBYTE)MapViewOfFile(
			m_hFileMap,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			0
		);
	}

	ShareMemory::~ShareMemory()
	{
		// 取消文件的映射，关闭文件映射对象句柄
		UnmapViewOfFile(m_pBuffer);
		CloseHandle(m_hFileMap);
	}
}