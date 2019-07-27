#pragma once
#include <vector>
#include <string>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <windows.h>

namespace ubase
{
	bool GetLocalMac(std::vector<std::string> &vsMacAddress);//获取本地所有网卡的MAC地址
};