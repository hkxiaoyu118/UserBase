#include "Error.h"

namespace ubase
{
	std::string GetErrorDescription(NTSTATUS code)
	{
		LPSTR lpMsgBuf = nullptr;

		if (FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			GetModuleHandleA("ntdll.dll"),
			code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)& lpMsgBuf, 0, NULL) != 0)
		{
			std::string ret(lpMsgBuf);

			LocalFree(lpMsgBuf);
			return ret;
		}

		return "";
	}
}
