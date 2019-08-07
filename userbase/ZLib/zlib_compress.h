#include <string>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include "zlib\\zconf.h"
#include "zlib\\zlib.h"
#ifdef _DEBUG
	#pragma comment(lib, "debug\\zlibstat.lib")
#else
	#pragma comment(lib, "release\\zlibstat.lib")
#endif

#define MAX_SRC_FILE_SIZE (100*1024*1024)			// 单个文件限制大小为 100M 

namespace ubase
{
	// 数据压缩
	// 输入：将要压缩文件的路径
	// 输出：数据压缩后的压缩数据内容、数据压缩后的压缩数据内容长度
	BOOL Zlib_CompressData(char* pszCompressFileName, BYTE** ppCompressData, DWORD* pdwCompressDataSize);


	// 数据解压
	// 输入：将要解压缩文件的路径
	// 输出：数据解压后的数据内容、数据解压后的内容长度
	BOOL Zlib_UncompressData(char* pszUncompressFileName, BYTE** ppUncompressData, DWORD* pdwUncompressDataSize);


	// 数据解压
	// 输入：将要解压缩的数据内容、将要解压缩的数据内容长度
	// 输出：数据解压后的数据内容、数据解压后的内容长度
	BOOL Zlib_UncompressData(BYTE* pCompressData, DWORD dwCompressDataSize, BYTE** ppUncompressData, DWORD* pdwUncompressDataSize);


	// 将数据存储为文件
	// 输入：数据原文件路径、将要保存的数据内容、将要保存的数据内容长度
	BOOL SaveToFile(char* pszFileName, BYTE* pData, DWORD dwDataSize);


	// 将数据存储为文件
	// 输入：数据原文件路径、将要保存的数据内容、将要保存的数据内容长度
	BOOL SaveToOriginalFile(char* pszFileName, BYTE* pData, DWORD dwDataSize);

	// 将字符串压缩
	// 输入：原始字符串
	// 输出：经过压缩后的二进制
	std::string CompressData(const std::string data);

	// 将二进制解压缩
	// 输入：压缩后的二进制
	// 输出：原始字符串
	std::string UnCompressData(const std::string data);
}
