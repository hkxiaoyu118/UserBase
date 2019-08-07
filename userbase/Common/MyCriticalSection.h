#pragma once
#include <windows.h>
class MyCriticalSection
{
public:
	MyCriticalSection(CRITICAL_SECTION* criticalSection)
	{
		m_criticalSection = criticalSection;
		::EnterCriticalSection(m_criticalSection);//进入临界区
	}
	~MyCriticalSection()
	{
		::LeaveCriticalSection(m_criticalSection);//退出临界区
	}
private:
	CRITICAL_SECTION* m_criticalSection;
};

