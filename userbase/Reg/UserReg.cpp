#include "../stdafx.h"
#include "UserReg.h"

namespace ubase
{
	bool RegSetAutoRun(std::string &filePath, std::string &valueName)
	{
		HKEY hKey;
		long ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			ret = RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE *)filePath.c_str(), (1 + filePath.length()));
			RegCloseKey(hKey);
			if (ret == ERROR_SUCCESS)
				return true;
		}
		return false;
	}
}

