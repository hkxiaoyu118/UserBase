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
		CHAR sectionName[9];
		DWORD virtualSize;
		DWORD virtualAddress;
		DWORD pointerToRawData;
		DWORD sizeOfRawData;
		DWORD characteristics;
	}PE_SECTION_INFO;

	typedef struct _PE_IMPORT_DESC
	{
		UINT dllIndex;
		CHAR dllName[MAX_PATH];
		DWORD originalFirstThunk;
		DWORD timeDateStamp;
		DWORD forwarderChain;
		DWORD firstThunk;
	}PE_IMPORT_DESC;

	typedef struct _PE_IMPORT_FUNC
	{
		DWORD thunkRVA;
		DWORD thunkValue;
		WORD hint;
		CHAR funcName[MAX_PATH];
	}PE_IMPORT_FUNC;

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
		std::string GetOrinalFunctionContent(PVOID imageBase, const char* findName, unsigned int count, bool bin2Hex = true);

		// 获取dll文件指定名称的函数的文件偏移
		// 如果获取成功,则返回非0
		DWORD PE::GetExportFunOffsetByName(PVOID imageBase, const char* findName);

		// 获取指定dll文件所有以命名方式的导出函数的函数名和文件偏移
		void PE::GetAllExportNamedFunctionsOffset(PVOID imageBase, std::map<std::string, DWORD>& mpFunctions);

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

		// 获取节块的基本信息
		bool GetSectionHeaderInfo(LPVOID imageBase, std::vector<PE_SECTION_INFO>& vtSectionInfo);

		// 获取数据目录项的入口地址
		LPVOID GetDirectoryEntryToData(LPVOID imageBase, USHORT directoryEntry);

		// 获取第一个导入表结构
		PIMAGE_IMPORT_DESCRIPTOR GetFirstImportDesc(LPVOID imageBase);

		// 获取导入表的基本信息
		bool GetImportDescInfo(LPVOID imageBase, std::vector<PE_IMPORT_DESC>& vtImportDesc);

		// 通过DllIndex获取导入函数列表
		bool GetImportFuncByDllIndex(LPVOID imageBase, UINT dllIndex, std::vector<PE_IMPORT_FUNC>& vtImportFunc);
	};
}
