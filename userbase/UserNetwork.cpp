#include "stdafx.h"
#include "UserNetwork.h"

#pragma comment(lib, "Iphlpapi.lib")

namespace ubase
{
	void Byte2Hex(unsigned char bData, unsigned char hex[])
	{
		int high = bData / 16, low = bData % 16;
		hex[0] = (high < 10) ? ('0' + high) : ('A' + high - 10);
		hex[1] = (low < 10) ? ('0' + low) : ('A' + low - 10);
	}

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
			//  pInfo->Address MACµÿ÷∑
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

		free(pInfo);// Õ∑≈ƒ⁄¥Ê

		if (vsMacAddress.size() > 0)
			return true;
		return false;
	}
}
