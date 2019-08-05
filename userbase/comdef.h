#pragma once
#include <windows.h>
#include <winternl.h>

#ifdef _WIN32                               //   ≈‰ Windows °£
typedef unsigned __int8     uint8;
typedef unsigned __int16    uint16;
typedef unsigned __int32    uint32;
typedef unsigned __int64    uint64;

typedef __int8              int8;
typedef __int16             int16;
typedef __int32             int32;
typedef __int64             int64;
#else                                       //   ≈‰ Linux °£
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;
#endif

typedef NTSTATUS(WINAPI *pZwQueryInformationProcess)(
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
	);