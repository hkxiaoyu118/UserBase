#pragma once
#include <vector>
#include <string>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <windows.h>
#include <tchar.h>

namespace ubase
{
	enum TCP_STATE
	{
		NONE,
		CLOSED,
		LISTENING,
		SYN_SENT,
		SEN_RECEIVED,
		ESTABLISHED,
		FIN_WAIT,
		FIN_WAIT2,
		CLOSE_WAIT,
		CLOSING,
		LAST_ACK,
		TIME_WAIT
	};

	bool GetLocalMac(std::vector<std::string> &vsMacAddress);//获取本地所有网卡的MAC地址
	int GetNetworkAdapter(std::vector<std::string> &vtMacs);//获取所有的真实网卡地址
	bool GetTcpConnect(std::vector<std::string> &tcp, TCP_STATE tcpstate, DWORD pid);//通过进程ID和TCP状态筛选出来连接
};