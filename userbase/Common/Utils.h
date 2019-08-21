#pragma once
#include <windows.h>

/*
	关闭当前线程的路径重定向(xp下不能使用,因为xp下没有Wow64机制)
*/
class FsRedirector
{
public:
	FsRedirector(bool wow64)
		: m_wow64(wow64)
	{
#ifndef XP_BUILD
		if (wow64)
			Wow64DisableWow64FsRedirection(&m_fsRedirection);
#endif
	}

	~FsRedirector()
	{
#ifndef XP_BUILD
		if (m_wow64)
			Wow64RevertWow64FsRedirection(m_fsRedirection);
#endif
	}

private:
	PVOID m_fsRedirection = nullptr;
	bool m_wow64;
};
