#include "../stdafx.h"
#include "UserNetwork.h"
#include "../String/UserString.h"

#pragma comment(lib, "Iphlpapi.lib")

namespace ubase
{
	bool GetLocalMac(std::vector<std::string> &vsMacAddress)
	{
		ULONG ulSize = 0;
		PIP_ADAPTER_INFO pInfo = NULL;
		int temp = 0;
		temp = GetAdaptersInfo(pInfo, &ulSize);
		pInfo = (PIP_ADAPTER_INFO)malloc(ulSize);
		temp = GetAdaptersInfo(pInfo, &ulSize);

		while (pInfo)
		{
			int iCount = 0;
			//  pInfo->Address MAC地址
			unsigned char mac[128] = { 0 };
			for (int i = 0; i < (int)pInfo->AddressLength; i++)
			{
				Byte2Hex(pInfo->Address[i], &mac[iCount]);
				iCount += 2;
				if (i < (int)pInfo->AddressLength - 1)
				{
					mac[iCount++] = ':';
				}
				else
				{
					std::string strMac = (char *)mac;
					vsMacAddress.push_back(strMac);
				}
			}
			pInfo = pInfo->Next;
		}

		free(pInfo);//释放内存

		if (vsMacAddress.size() > 0)
			return true;
		return false;
	}

	BOOL IsLocalAdapter(LPSTR szName)
	{
		if (!szName)
		{
			return FALSE;
		}
		TCHAR szKeyName[260] = _T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}");
		HKEY hSubKey = NULL;
		LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKeyName, 0, KEY_READ, &hSubKey);
		if (lRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
		CHAR szDataBuf[260] = { 0 };
		sprintf_s(szDataBuf, "%s\\Connection", szName);//格式化字符串
		//打开格式化后的字符串
		HKEY hLocalKey = NULL;
		lRet = RegOpenKeyExA(hSubKey, szDataBuf, 0, KEY_READ, &hLocalKey);
		if (ERROR_SUCCESS != lRet)
		{
			RegCloseKey(hSubKey);
			return FALSE;
		}
		//请求注册表中的值
		DWORD dwType = REG_SZ;
		TCHAR szData[250];
		DWORD dwSize = 250;
		lRet = RegQueryValueEx(hLocalKey, _T("PnPInstanceId"), 0, &dwType, (BYTE*)(szData), &dwSize);
		if (ERROR_SUCCESS != lRet)
		{
			RegCloseKey(hSubKey);
			RegCloseKey(hLocalKey);
			return FALSE;
		}
		if (0 == _tcsncmp(szData, _T("PCI"), _tcslen(_T("PCI"))) || 0 == _tcsncmp(szData, _T("USB"), _tcslen(_T("USB"))))
		{
			RegCloseKey(hSubKey);
			RegCloseKey(hLocalKey);
			return TRUE;
		}
		RegCloseKey(hSubKey);
		RegCloseKey(hLocalKey);
		return FALSE;
	}

	std::string ConvertToMacAddress(unsigned char* addrbytes, unsigned length, bool bIsUpper)
	{
		std::string strHex;
		for (int i = 0; i < length; i++)
		{
			if (i != 0)
			{
				strHex = strHex + ":";
			}
			char szTemp[4] = { 0 };
			sprintf(szTemp, "%02x", addrbytes[i]);
			strHex = strHex + szTemp;
		}
		return strHex;
	}

	//获取网络适配器的数量
	int GetNetworkAdapter(std::vector<std::string> &vtMacs)
	{
		PIP_ADAPTER_INFO pAdapterInfo = NULL;
		ULONG OutBufLen = 0;
		int nCount = 0;

		//Get the required size of the buffer
		if (GetAdaptersInfo(NULL, &OutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			int divisor = sizeof IP_ADAPTER_INFO;
			if (sizeof time_t == 8)
				divisor -= 8;

			nCount = OutBufLen / divisor;
			pAdapterInfo = new IP_ADAPTER_INFO[nCount];

			//Get info for the adapters
			if (GetAdaptersInfo(pAdapterInfo, &OutBufLen) != ERROR_SUCCESS)
			{
				//Call failed
				delete[] pAdapterInfo;
				pAdapterInfo = NULL;
				nCount = 0;
			}
		}
		int nResult = 0;//最终物理网卡的数量
		for (int i = 0; i < nCount; ++i)
		{
			if (IsLocalAdapter(pAdapterInfo->AdapterName))
			{
				std::string macAddress = ConvertToMacAddress(pAdapterInfo->Address, pAdapterInfo->AddressLength, false);
				vtMacs.push_back(macAddress);
				++nResult;
			}
			pAdapterInfo = pAdapterInfo->Next;
		}
		delete[]pAdapterInfo;
		return nResult;
	}

	bool GetTcpConnect(std::vector<std::string> &tcp, TCP_STATE tcpstate, DWORD pid)
	{
		PMIB_TCPTABLE_OWNER_PID pTcpTable(NULL);
		DWORD dwSize(0);
		if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER)
		{
			pTcpTable = (MIB_TCPTABLE_OWNER_PID *)new char[dwSize];//重新分配缓冲区

			if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) != NO_ERROR)
			{
				delete pTcpTable;
				return false;
			}
			int nNum = (int)pTcpTable->dwNumEntries; //TCP连接的数目
			for (int i = 0; i < nNum; i++)
			{
				if (pid == pTcpTable->table[i].dwOwningPid)
				{
					if (pTcpTable->table[i].dwState == tcpstate)
					{
						char tcpInfo[64] = { 0 };
						sprintf(tcpInfo, "%s:%d", inet_ntoa(*(in_addr*)& pTcpTable->table[i].dwLocalAddr), htons(pTcpTable->table[i].dwLocalPort));
						tcp.push_back(tcpInfo);
					}
				}
			}
		}
		delete pTcpTable;
		return true;
	}
}
