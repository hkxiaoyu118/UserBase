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

	bool PE::IsPEFile(LPVOID imageBase)
	{
		PIMAGE_DOS_HEADER pDosHeader = NULL;
		PIMAGE_NT_HEADERS pNtHeader = NULL;
		if (imageBase == NULL)
			return false;

		pDosHeader = (PIMAGE_DOS_HEADER)imageBase;//获取DOS头部
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)//判断是否是MZ
			return false;

		pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);//获取NT头部
		if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)//判断是否为PE格式
			return false;

		return true;
	}

	PIMAGE_DOS_HEADER PE::GetDosHeader(LPVOID imageBase)
	{
		PIMAGE_DOS_HEADER pDosHeader = NULL;

		if (!IsPEFile(imageBase))
			return NULL;

		pDosHeader = (PIMAGE_DOS_HEADER)imageBase;
		return pDosHeader;
	}

	PIMAGE_NT_HEADERS PE::GetNtHeaders(LPVOID imageBase)
	{
		PIMAGE_DOS_HEADER pDosHeader = NULL;
		PIMAGE_NT_HEADERS pNtHeader = NULL;

		if (!IsPEFile(imageBase))
			return NULL;

		pDosHeader = (PIMAGE_DOS_HEADER)imageBase;
		pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
		return pNtHeader;
	}

	PIMAGE_FILE_HEADER PE::GetFileHeader(LPVOID imageBase)
	{
		PIMAGE_NT_HEADERS pNtHeader = NULL;
		PIMAGE_FILE_HEADER pFileHeader = NULL;
		pNtHeader = GetNtHeaders(imageBase);
		if (!pNtHeader)
			return NULL;
		pFileHeader = &pNtHeader->FileHeader;
		return pFileHeader;
	}

	PIMAGE_OPTIONAL_HEADER PE::GetOptionalHeader(LPVOID imageBase)
	{
		PIMAGE_NT_HEADERS pNtHeader = NULL;
		PIMAGE_OPTIONAL_HEADER pOptionalHeader = NULL;
		pNtHeader = GetNtHeaders(imageBase);
		if (!pNtHeader)
			return NULL;
		pOptionalHeader = &pNtHeader->OptionalHeader;
		return pOptionalHeader;
	}

	PIMAGE_SECTION_HEADER PE::GetFirstSectionHeader(PIMAGE_NT_HEADERS pNtHeader)
	{
		PIMAGE_SECTION_HEADER pSectionHeader = NULL;
		pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
		return pSectionHeader;
	}

	bool PE::GetSectionHeaderInfo(LPVOID imageBase, std::vector<PE_SECTION_INFO>& vtSectionInfo)
	{
		PIMAGE_NT_HEADERS pNtHeader = GetNtHeaders(imageBase);
		if (!pNtHeader)
			return false;

		PIMAGE_FILE_HEADER pFileHeader = &pNtHeader->FileHeader;
		if (!pFileHeader)
			return false;

		PIMAGE_SECTION_HEADER pSectionHeader = GetFirstSectionHeader(pNtHeader);
		if (!pSectionHeader)
			return false;

		for (int i = 0; i < pFileHeader->NumberOfSections; i++)
		{
			PE_SECTION_INFO item = { 0 };

			//节名称
			char cName[9];
			memset(cName, 0, sizeof(cName));
			memcpy(cName, pSectionHeader->Name, 8);

			//真实长度
			DWORD virtualSize = pSectionHeader->Misc.VirtualSize;
			//节虚拟偏移(RVA)
			DWORD virtualAddress = pSectionHeader->VirtualAddress;
			//节文件偏移(Offset)
			DWORD pointerToRawData = pSectionHeader->PointerToRawData;
			//节大小
			DWORD sizeOfRawData = pSectionHeader->SizeOfRawData;
			//节属性,可读、可写、可执行
			DWORD characteristics = pSectionHeader->Characteristics;

			memcpy(item.sectionName, cName, 8);
			item.virtualSize = virtualSize;
			item.virtualAddress = virtualAddress;
			item.pointerToRawData = pointerToRawData;
			item.sizeOfRawData = sizeOfRawData;
			item.characteristics = characteristics;

			vtSectionInfo.push_back(item);

			++pSectionHeader;
		}

		if (vtSectionInfo.size() != 0)
			return true;

		return false;
	}
}