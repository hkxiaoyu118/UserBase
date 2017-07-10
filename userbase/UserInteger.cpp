#include "stdafx.h"
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
}
