#pragma once
#include <windows.h>
#include <string>

namespace ubase
{
	bool RegSetAutoRun(std::string &filePath, std::string &valueName);//设置程序的自动启动
}
