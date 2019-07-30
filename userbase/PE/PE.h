#pragma once
#include <string>
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
	};
}
