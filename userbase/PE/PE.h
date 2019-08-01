#pragma once
#include <string>
#include <map>
#include <vector>
#include <windows.h>

namespace ubase
{
	typedef struct _MAP_FILE_STRUCT
	{
		HANDLE hFile;//文件句柄
		HANDLE hMapping;//映射文件句柄
		LPVOID imageBase;//映像地址
	}MAP_FILE_STRUCT;

	typedef struct _PE_SECTION_INFO
	{
		char sectionName[9];
		DWORD virtualSize;
		DWORD virtualAddress;
		DWORD pointerToRawData;
		DWORD sizeOfRawData;
		DWORD characteristics;
	}PE_SECTION_INFO;

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

		// 判断是否是PE格式
		bool IsPEFile(LPVOID imageBase);

		// 获取DOS头部
		PIMAGE_DOS_HEADER GetDosHeader(LPVOID imageBase);

		// 获取NT头部
		PIMAGE_NT_HEADERS GetNtHeaders(LPVOID imageBase);

		// 获取文件头
		PIMAGE_FILE_HEADER GetFileHeader(LPVOID imageBase);

		// 获取可选头
		PIMAGE_OPTIONAL_HEADER GetOptionalHeader(LPVOID imageBase);

		// 获取第一个节的位置
		PIMAGE_SECTION_HEADER GetFirstSectionHeader(PIMAGE_NT_HEADERS pNtHeader);

		//获取节块的基本信息
		bool GetSectionHeaderInfo(LPVOID imageBase, std::vector<PE_SECTION_INFO>& vtSectionInfo);//获取节的信息
	};
}
