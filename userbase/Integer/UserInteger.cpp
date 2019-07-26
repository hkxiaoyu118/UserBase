#include "../stdafx.h"
#include "UserInteger.h"

namespace ubase
{
	bool IntGetRandomUInt(unsigned int &randomNum)
	{
		bool result = false;
		HCRYPTPROV hProvider;
		BYTE buffer[4];
		if (::CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		{
			if (::CryptGenRandom(hProvider, 4, buffer))
			{
				randomNum = *(unsigned short*)buffer;
				result = true;
			}
			::CryptReleaseContext(hProvider, 0);
		}
		return result;
	}

	void ConvertUIntToBytes(unsigned int number, unsigned char numBytes[], bool bigEndian)
	{
		uint8_t result[4] = { 0 };//存储结果
		uint8_t l1 = number & 0x000000FF;
		uint8_t l2 = (number & 0x0000FF00) >> 8;
		uint8_t l3 = (number & 0x00FF0000) >> 16;
		uint8_t l4 = (number & 0xFF000000) >> 24;

		if (bigEndian == true)//如果是大端序
		{
			numBytes[0] = l4;
			numBytes[1] = l3;
			numBytes[2] = l2;
			numBytes[3] = l1;
		}
		else//如果是小端序
		{
			numBytes[0] = l1;
			numBytes[1] = l2;
			numBytes[2] = l3;
			numBytes[3] = l4;
		}
	}

	unsigned int ConvertBytesToUInt(unsigned char numBytes[], bool bigEndian)
	{
		unsigned int number = 0;
		unsigned int l1 = numBytes[0];
		unsigned int l2 = numBytes[1];
		unsigned int l3 = numBytes[2];
		unsigned int l4 = numBytes[3];

		if (bigEndian == true)
		{
			number = l1 << 24;
			number += l2 << 16;
			number += l3 << 8;
			number += l4;
		}
		else
		{
			number = l4 << 24;
			number += l3 << 16;
			number += l2 << 8;
			number += l1;
		}
		return number;
	}
}
