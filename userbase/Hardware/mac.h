#include <vector>
#include <string>
#include <windows.h>
#include <Iphlpapi.h>
#include <tchar.h>


int GetNetworkAdapter(std::vector<std::string> &vtMacs); //获取真实的MAC地址
