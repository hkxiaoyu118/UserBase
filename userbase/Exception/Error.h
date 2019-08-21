#pragma once
#include <string>
#include <windows.h>

namespace ubase
{
	std::string GetErrorDescription(NTSTATUS code);//根据GetLastError获取错误信息
}
