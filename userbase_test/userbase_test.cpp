#include <tchar.h>
#define _STRSAFE_H_INCLUDED_
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0501

#include <iostream>
#include "../userbase/ubase.h"
#include <map>
#include <vector>

#pragma comment(lib, "../Release/Bin/userbase.lib")

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "D:/CodeYuyincun/userbase/userbase/Lib/libcrypto.lib")

//#define _CRT_SECURE_NO_WARNINGS

void __stdcall MyProcessRecvData(char* data, unsigned int dataLength)
{
	std::string message = data;
	std::cout << message << std::endl;
}

// int main()
// {
// 	InitLog(0, 0, 0, 0, NULL, NULL);
// 	std::string key = "www.yuyincun.com";//加解密密钥
// 	std::string ip = "127.0.0.1";
// 	unsigned int port = 9988;
// 	TcpClientGo go(MyProcessRecvData, ip, port, key);
// 	unsigned int index = 0;
// 	while (true)
// 	{
// 		system("pause");
// 		std::string loginText = "{\"type\":\"login\",\"data\":{\"login_name\":\"hkxiaoyu118\"}}";
// 		bool result=go.SendData(loginText);
// 		if (result == true)
// 		{
// 			std::cout << "发送login数据成功" << std::endl;
// 		}
// 		else
// 		{
// 			std::cout << "发送login数据失败" << std::endl;
// 		}
// 
// 		system("pause");
// 		std::string msgText = "{\"type\":\"message\",\"data\":{\"login_name\":\"hkxiaoyu118\", \"message\":\"this TEST " + std::to_string(index) + "\"}}";
// 		index++;
// 		result = go.SendData(msgText);
// 		if (result == true)
// 		{
// 			std::cout << "发送login数据成功" << std::endl;
// 		}
// 		else
// 		{
// 			std::cout << "发送login数据失败" << std::endl;
// 		}
// 	}
// 	system("pause");
// }


int main()
{
// 	std::vector<TrayInfo> trayInfo;
// 	TrayWindow win;
// 	HWND hwnd1=win.FindNotifyIconOverflowWindow();
// 	win.EnumNotifyWindow(hwnd1, trayInfo);
// 
// 	for (auto item = trayInfo.begin(); item != trayInfo.end(); item++)
// 	{
// 		printf("%d %d %d %s %s\n", item->rect.left, item->rect.top, item->pid, item->title.c_str(),  item->filePath.c_str());
// 	}

// 	HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, 15024);
// 	std::string msg = ubase::PsGetProcessFullPathV2(handle);
// 	std::cout << msg << std::endl;

	std::string filePath = "D:\\Code\\TestApp\\Aliwangwang\\AliIM.exe";
	if (PsAutoRunStartup((char *)filePath.c_str(), "测试") == TRUE)
	{
		std::cout << "PsAutoRunStartup成功" << std::endl;
		PsCreateDesktopLink((char*)filePath.c_str(), "hahahha哈哈");
	}
	else
	{
		std::cout << "PsAutoRunStartup失败" << std::endl;
	}
	system("pause");
}