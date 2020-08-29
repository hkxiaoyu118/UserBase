#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wintrust")

namespace ubase
{
	BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext, TCHAR *objectName);
}
