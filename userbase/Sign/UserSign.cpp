#include "UserSign.h"

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)


namespace ubase
{
	VOID GetVeriyObjectName(CONST TCHAR *fileName, TCHAR *objectName)
	{
		HCERTSTORE hStore = NULL;
		HCRYPTMSG hMsg = NULL;
		PCCERT_CONTEXT pCertContext = NULL;
		BOOL fResult;
		DWORD dwEncoding, dwContentType, dwFormatType;
		PCMSG_SIGNER_INFO pSignerInfo = NULL;
		PCMSG_SIGNER_INFO pCounterSignerInfo = NULL;
		DWORD dwSignerInfo;
		CERT_INFO CertInfo;

		__try
		{
			// Get message handle and store handle from the signed file.
			fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
				fileName,
				CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
				CERT_QUERY_FORMAT_FLAG_BINARY,
				0,
				&dwEncoding,
				&dwContentType,
				&dwFormatType,
				&hStore,
				&hMsg,
				NULL);
			if (!fResult)
			{
				__leave;
			}
			// Get signer information size.
			fResult = CryptMsgGetParam(hMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				NULL,
				&dwSignerInfo);
			if (!fResult)
			{
				__leave;
			}
			// Allocate memory for signer information.
			pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
			if (!pSignerInfo)
			{
				__leave;
			}
			// Get Signer Information.
			fResult = CryptMsgGetParam(hMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				(PVOID)pSignerInfo,
				&dwSignerInfo);
			if (!fResult)
			{
				__leave;
			}

			// Search for the signer certificate in the temporary
			// certificate store.
			CertInfo.Issuer = pSignerInfo->Issuer;
			CertInfo.SerialNumber = pSignerInfo->SerialNumber;
			pCertContext = CertFindCertificateInStore(hStore,
				ENCODING,
				0,
				CERT_FIND_SUBJECT_CERT,
				(PVOID)&CertInfo,
				NULL);
			if (!pCertContext)
			{
				__leave;
			}
			// Print Signer certificate information.
			PrintCertificateInfo(pCertContext, objectName);
		}
		__finally
		{
			if (pSignerInfo != NULL) LocalFree(pSignerInfo);
			if (pCounterSignerInfo != NULL) LocalFree(pCounterSignerInfo);
			if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
			if (hStore != NULL) CertCloseStore(hStore, 0);
			if (hMsg != NULL) CryptMsgClose(hMsg);
		}
	}

	BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext, TCHAR *objectName)
	{
		BOOL fReturn = FALSE;
		LPTSTR szName = NULL;
		DWORD dwData;
		__try
		{
			// Get Issuer name size.
			if (!(dwData = CertGetNameString(pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				CERT_NAME_ISSUER_FLAG,
				NULL,
				NULL,
				0)))
			{
				__leave;
			}
			// Allocate memory for Issuer name.
			szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
			if (!szName)
			{
				__leave;
			}
			// Get Issuer name.
			if (!(CertGetNameString(pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				CERT_NAME_ISSUER_FLAG,
				NULL,
				szName,
				dwData)))
			{
				__leave;
			}
			LocalFree(szName);
			szName = NULL;
			// Get Subject name size.
			if (!(dwData = CertGetNameString(pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0,
				NULL,
				NULL,
				0)))
			{
				__leave;
			}
			// Allocate memory for subject name.
			szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
			if (!szName)
			{
				__leave;
			}
			// Get subject name.
			if (!(CertGetNameString(pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0,
				NULL,
				szName,
				dwData)))
			{
				__leave;
			}
			strcpy_s(objectName, MAX_PATH, szName);
			fReturn = TRUE;
		}
		__finally
		{
			if (szName != NULL) LocalFree(szName);
		}
		return fReturn;
	}
}
