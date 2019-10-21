#ifndef _HKXIAOYU118_SERVICE_MANAGER_H_
#define _HKXIAOYU118_SERVICE_MANAGER_H_


#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


class ServiceManager
{
public:
	ServiceManager();
	~ServiceManager();

public:

	/*
		0 加载服务
		1 启动服务
		2 停止服务
		3 删除服务
	*/
	void ShowError(char* lpszText);
	BOOL SystemServiceCtrl(char* lpszExePath, int iOperateType);
};

#endif