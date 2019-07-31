#pragma once
#include <string>
#include <map>
#include <windows.h>

namespace ubase
{
	class PE
	{
	public:
		PE();
		~PE();

		// 内存偏移转文件偏移(如果成功返回非0)
		DWORD RvaToOffset(PIMAGE_NT_HEADERS pNTHeader, DWORD dwExportRVA);

		// 获取DLL导出函数的原始内容
		// pFile:dll文件的二进制内容
		// count:需要读取函数的字节数(从函数头开始)
		// bin2Hex:读取结果是否以16进制的方式展示
		std::string GetOrinalFunctionContent(const char* pFile, const char* findName, unsigned int count, bool bin2Hex = true);

		// 获取dll文件指定名称的函数的文件偏移
		// 如果获取成功,则返回非0
		DWORD PE::GetExportFunOffsetByName(const char* pFile, const char* findName);

		// 获取指定dll文件所有以命名方式的导出函数的函数名和文件偏移
		void PE::GetAllExportNamedFunctionsOffset(const char* pFile, std::map<std::string, DWORD>& mpFunctions);
	};
}
