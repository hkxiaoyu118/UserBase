#include "PE.h"
#include "../String/UserString.h"

namespace ubase
{
	PE::PE()
	{

	}

	PE::~PE()
	{

	}

	DWORD PE::RvaToOffset(PIMAGE_NT_HEADERS pNTHeader, DWORD dwExportRVA)
	{
		PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((DWORD)pNTHeader + sizeof(IMAGE_NT_HEADERS));
		for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
		{
			if (dwExportRVA >= pSection[i].VirtualAddress && dwExportRVA < (pSection[i].VirtualAddress + pSection[i].SizeOfRawData))
			{
				return pSection[i].PointerToRawData + (dwExportRVA - pSection[i].VirtualAddress);
			}
		}
		return 0;
	}

	std::string PE::GetOrinalFunctionContent(const char* pFile, const char* findName, unsigned int count, bool bin2Hex)
	{
		std::string result;
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFile;//获取DOS头
		PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)(pFile + pDosHeader->e_lfanew);//获取NT头
		DWORD dwExportOffset = RvaToOffset(pNTHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (dwExportOffset != 0)
		{
			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(pFile + dwExportOffset);
			DWORD dwFunNameOffset = (DWORD)pFile + RvaToOffset(pNTHeader, pExport->Name);
			DWORD* pDwNamesAddress = (DWORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfNames));
			DWORD* pdwFunctionAddress = (DWORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfFunctions));
			WORD* pwOrdinals = (WORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfNameOrdinals));

			for (int i = 0; i < pExport->NumberOfNames; i++)
			{
				DWORD dwFunctionAddress = pdwFunctionAddress[pwOrdinals[i]];
				DWORD pdwFunNameOffset = RvaToOffset(pNTHeader, pDwNamesAddress[i]);
				char* funName = (char*)(pFile + pdwFunNameOffset);//函数名称
				//对比函数名称是否是我们想找的函数名
				if (_stricmp(funName, findName) == 0)//如果函数是我们想找的函数
				{
					DWORD dwFunctionOffset = RvaToOffset(pNTHeader, dwFunctionAddress);//获取导出函数的文件偏移
					result.resize(count);
					memcpy((char*)result.c_str(), (char*)(pFile + dwFunctionOffset), count);
					if (bin2Hex == true)
					{
						result = ubase::BinToHex(result, true);
					}
					break;
				}
			}
		}
		return result;
	}

	DWORD PE::GetExportFunOffsetByName(const char* pFile, const char* findName)
	{
		DWORD offset = 0;
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFile;//获取DOS头
		PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)(pFile + pDosHeader->e_lfanew);//获取NT头
		DWORD dwExportOffset = RvaToOffset(pNTHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (dwExportOffset != 0)
		{
			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(pFile + dwExportOffset);
			DWORD dwFunNameOffset = (DWORD)pFile + RvaToOffset(pNTHeader, pExport->Name);
			DWORD* pDwNamesAddress = (DWORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfNames));
			DWORD* pdwFunctionAddress = (DWORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfFunctions));
			WORD* pwOrdinals = (WORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfNameOrdinals));

			for (int i = 0; i < pExport->NumberOfNames; i++)
			{
				DWORD dwFunctionAddress = pdwFunctionAddress[pwOrdinals[i]];
				DWORD pdwFunNameOffset = RvaToOffset(pNTHeader, pDwNamesAddress[i]);
				char* funName = (char*)(pFile + pdwFunNameOffset);//函数名称
				//对比函数名称是否是我们想找的函数名
				if (_stricmp(funName, findName) == 0)//如果函数是我们想找的函数
				{
					offset = RvaToOffset(pNTHeader, dwFunctionAddress);//获取查找的导出函数的文件偏移
					break;
				}
			}
		}
		return offset;
	}

	void PE::GetAllExportNamedFunctionsOffset(const char* pFile, std::map<std::string, DWORD>& mpFunctions)
	{
		bool result = false;
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFile;//获取DOS头
		PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)(pFile + pDosHeader->e_lfanew);//获取NT头
		DWORD dwExportOffset = RvaToOffset(pNTHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (dwExportOffset != 0)
		{
			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(pFile + dwExportOffset);
			DWORD dwFunNameOffset = (DWORD)pFile + RvaToOffset(pNTHeader, pExport->Name);
			DWORD* pDwNamesAddress = (DWORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfNames));
			DWORD* pdwFunctionAddress = (DWORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfFunctions));
			WORD* pwOrdinals = (WORD*)(pFile + RvaToOffset(pNTHeader, pExport->AddressOfNameOrdinals));

			for (int i = 0; i < pExport->NumberOfNames; i++)
			{
				DWORD dwFunctionAddress = pdwFunctionAddress[pwOrdinals[i]];//获取对应名称函数的RVA
				DWORD pdwFunNameOffset = RvaToOffset(pNTHeader, pDwNamesAddress[i]);//获取存储函数名称的RVA
				char* funName = (char*)(pFile + pdwFunNameOffset);//获取函数的名称
				DWORD fileOffset= RvaToOffset(pNTHeader, dwFunctionAddress);//获取函数的文件偏移
				mpFunctions[funName] = fileOffset;//存储命名导出函数的信息
			}
		}
	}
}