#pragma once
#include <windows.h>

namespace ubase
{
	class Event
	{
	public:
		Event(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR lpName = NULL);
		~Event();
		bool Wait(const DWORD time = INFINITE);	//等待事件
		bool Set();								//激发事件
		bool Reset();							//重置事件(设置事件未激发,手动模式使用)
		bool IsNamedEventExist();				//判断命名事件是否已经存在
		DWORD GetEventLastError();				//获取错误
	private:
		HANDLE m_handle;						//事件句柄
		bool m_isNamedEventExist;				//命名事件是否已经存在
		DWORD m_dwErr;							//错误代码
	};

	Event::Event(BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName):m_isNamedEventExist(false), m_dwErr(0)
	{
		m_handle = CreateEvent(NULL, bManualReset, bInitialState, lpName);
		m_dwErr = GetLastError();

		if (m_dwErr == ERROR_ALREADY_EXISTS)	//判断事件是否存在
		{
			m_isNamedEventExist = true;
		}
	}

	Event::~Event()
	{
		if (m_handle != NULL)
		{
			CloseHandle(m_handle);
			m_handle = NULL;
		}
	}

	bool Event::Wait(const DWORD time /* = INFINITE */)
	{
		return WAIT_OBJECT_0 == WaitForSingleObject(m_handle, time);
	}

	bool Event::Set()
	{
		return SetEvent(m_handle) == TRUE;
	}

	bool Event::Reset()
	{
		return ResetEvent(m_handle) == TRUE;
	}

	bool Event::IsNamedEventExist()
	{
		return m_isNamedEventExist;
	}

	DWORD Event::GetEventLastError()
	{
		return m_dwErr == ERROR_ALREADY_EXISTS ? 0 : m_dwErr;
	}
}