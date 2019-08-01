#include "PE.h"
#include "../String/UserString.h"
#include <imagehlp.h>

#pragma comment(lib, "Imagehlp.lib")

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

	std::string PE::GetOrinalFunctionContent(PVOID imageBase, const char* findName, unsigned int count, bool bin2Hex)
	{
		std::string result;
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)imageBase;//获取DOS头
		PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)((DWORD)imageBase + pDosHeader->e_lfanew);//获取NT头
		DWORD dwExportOffset = RvaToOffset(pNTHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (dwExportOffset != 0)
		{
			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)imageBase + dwExportOffset);
			DWORD dwFunNameOffset = (DWORD)imageBase + RvaToOffset(pNTHeader, pExport->Name);
			DWORD* pDwNamesAddress = (DWORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfNames));
			DWORD* pdwFunctionAddress = (DWORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfFunctions));
			WORD* pwOrdinals = (WORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfNameOrdinals));

			for (int i = 0; i < pExport->NumberOfNames; i++)
			{
				DWORD dwFunctionAddress = pdwFunctionAddress[pwOrdinals[i]];
				DWORD pdwFunNameOffset = RvaToOffset(pNTHeader, pDwNamesAddress[i]);
				char* funName = (char*)((DWORD)imageBase + pdwFunNameOffset);//函数名称
				//对比函数名称是否是我们想找的函数名
				if (_stricmp(funName, findName) == 0)//如果函数是我们想找的函数
				{
					DWORD dwFunctionOffset = RvaToOffset(pNTHeader, dwFunctionAddress);//获取导出函数的文件偏移
					result.resize(count);
					memcpy((char*)result.c_str(), (char*)((DWORD)imageBase + dwFunctionOffset), count);
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

	DWORD PE::GetExportFunOffsetByName(PVOID imageBase, const char* findName)
	{
		DWORD offset = 0;
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)imageBase;//获取DOS头
		PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)((DWORD)imageBase + pDosHeader->e_lfanew);//获取NT头
		DWORD dwExportOffset = RvaToOffset(pNTHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (dwExportOffset != 0)
		{
			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)imageBase + dwExportOffset);
			DWORD dwFunNameOffset = (DWORD)imageBase + RvaToOffset(pNTHeader, pExport->Name);
			DWORD* pDwNamesAddress = (DWORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfNames));
			DWORD* pdwFunctionAddress = (DWORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfFunctions));
			WORD* pwOrdinals = (WORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfNameOrdinals));

			for (int i = 0; i < pExport->NumberOfNames; i++)
			{
				DWORD dwFunctionAddress = pdwFunctionAddress[pwOrdinals[i]];
				DWORD pdwFunNameOffset = RvaToOffset(pNTHeader, pDwNamesAddress[i]);
				char* funName = (char*)((DWORD)imageBase + pdwFunNameOffset);//函数名称
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

	void PE::GetAllExportNamedFunctionsOffset(PVOID imageBase, std::map<std::string, DWORD>& mpFunctions)
	{
		bool result = false;
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)imageBase;//获取DOS头
		PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)((DWORD)imageBase + pDosHeader->e_lfanew);//获取NT头
		DWORD dwExportOffset = RvaToOffset(pNTHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (dwExportOffset != 0)
		{
			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)imageBase + dwExportOffset);
			DWORD dwFunNameOffset = (DWORD)imageBase + RvaToOffset(pNTHeader, pExport->Name);
			DWORD* pDwNamesAddress = (DWORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfNames));
			DWORD* pdwFunctionAddress = (DWORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfFunctions));
			WORD* pwOrdinals = (WORD*)((DWORD)imageBase + RvaToOffset(pNTHeader, pExport->AddressOfNameOrdinals));

			for (int i = 0; i < pExport->NumberOfNames; i++)
			{
				DWORD dwFunctionAddress = pdwFunctionAddress[pwOrdinals[i]];//获取对应名称函数的RVA
				DWORD pdwFunNameOffset = RvaToOffset(pNTHeader, pDwNamesAddress[i]);//获取存储函数名称的RVA
				char* funName = (char*)((DWORD)imageBase + pdwFunNameOffset);//获取函数的名称
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

	LPVOID PE::GetDirectoryEntryToData(LPVOID imageBase, USHORT directoryEntry)
	{
		DWORD dwDataStartRVA;
		LPVOID pDirData = NULL;
		PIMAGE_NT_HEADERS     pNtHeader = NULL;
		PIMAGE_OPTIONAL_HEADER pOptionalHeader = NULL;

		pNtHeader = GetNtHeaders(imageBase);
		if (!pNtHeader)
			return NULL;
		pOptionalHeader = GetOptionalHeader(imageBase);
		if (!pOptionalHeader)
			return NULL;
		dwDataStartRVA = pOptionalHeader->DataDirectory[directoryEntry].VirtualAddress;
		if (!dwDataStartRVA)
			return NULL;

		pDirData = ImageRvaToVa(pNtHeader, imageBase, dwDataStartRVA, NULL);
		if (!pDirData)
			return NULL;
		return  pDirData;
	}

	PIMAGE_IMPORT_DESCRIPTOR PE::GetFirstImportDesc(LPVOID imageBase)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)GetDirectoryEntryToData(imageBase, IMAGE_DIRECTORY_ENTRY_IMPORT);
		if (!pImportDesc)
			return NULL;
		return pImportDesc;
	}

	bool PE::GetImportDescInfo(LPVOID imageBase, std::vector< PE_IMPORT_DESC>& vtImportDesc)
	{
		bool result = false;
		PIMAGE_NT_HEADERS pNtHeader = NULL;
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;

		pNtHeader = GetNtHeaders(imageBase);
		if (!pNtHeader)
			return result;

		pImportDesc = GetFirstImportDesc(imageBase);
		if (!pImportDesc)
			return result;

		while (pImportDesc->FirstThunk)
		{
			PE_IMPORT_DESC item = { 0 };

			char* szDllName = (char*)ImageRvaToVa(pNtHeader, imageBase, pImportDesc->Name, NULL);//获取导入DLL名称
			DWORD originalFirstThunk = pImportDesc->OriginalFirstThunk;
			DWORD timeDateStamp = pImportDesc->TimeDateStamp;
			DWORD forwarderChain = pImportDesc->ForwarderChain;
			DWORD firstThunk = pImportDesc->FirstThunk;
			
			strcpy(item.dllName, szDllName);
			item.originalFirstThunk = originalFirstThunk;
			item.timeDateStamp = timeDateStamp;
			item.forwarderChain = forwarderChain;
			item.firstThunk = firstThunk;

			vtImportDesc.push_back(item);

			pImportDesc++;
		}

		if (vtImportDesc.size() != 0)
			result = true;

		return result;
	}
}