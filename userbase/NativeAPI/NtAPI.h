#pragma once
#include <windows.h>
#include <winternl.h>

NTSTATUS WINAPI ZwQuerySystemInformation(
	__in       SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout    PVOID                    SystemInformation,
	__in       ULONG_PTR                SystemInformationLength,
	__out_opt  PULONG_PTR               ReturnLength
);

NTSTATUS WINAPI ZwProtectVirtualMemory(
	__in_opt   HANDLE       ProcessHandle,
	__inout    PVOID* BaseAddress,
	__inout    PULONG_PTR   NumberOfBytesToProtect,
	__in       ULONG_PTR    NewAccessProtection,
	__out      PULONG_PTR   OldAccessProtection
);

VOID WINAPI RtlInitUnicodeString(
	__out     PUNICODE_STRING DestinationString,
	__in_opt  PCWSTR          SourceString
);

VOID WINAPI RtlInitAnsiString(
	__out     PANSI_STRING  DestinationString,
	__in_opt  PCSZ          SourceString
);

NTSTATUS WINAPI RtlUnicodeStringToAnsiString(
	__inout PANSI_STRING      DestinationString,
	__in    PCUNICODE_STRING  SourceString,
	__in    BOOLEAN           AllocateDestinationString
);

NTSTATUS WINAPI RtlAnsiStringToUnicodeString(
	__inout PUNICODE_STRING DestinationString,
	__in    PCANSI_STRING   SourceString,
	__in    BOOLEAN         AllocateDestinationString
);

VOID WINAPI RtlFreeUnicodeString(
	__inout PUNICODE_STRING UnicodeString
);

VOID WINAPI RtlFreeAnsiString(
	__inout PANSI_STRING AnsiString
);

NTSTATUS WINAPI ZwQueryInformationProcess(
	__in HANDLE             ProcessHandle,
	__in PROCESSINFOCLASS   ProcessInformationClass,
	__out_bcount_opt(ProcessInformationLength) PVOID ProcessInformation,
	__in ULONG_PTR          ProcessInformationLength,
	__out_opt PULONG_PTR    ReturnLength
);

NTSTATUS WINAPI ZwQueryObject(
	_In_opt_ HANDLE               Handle,
	_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_writes_bytes_opt_(ObjectInformationLength) PVOID ObjectInformation,
	_In_ ULONG                    ObjectInformationLength,
	_Out_opt_ PULONG              ReturnLength
);

NTSTATUS WINAPI LdrLoadDll(
	IN  PWCHAR            PathToFile,
	IN  ULONG             Flags,
	IN  PUNICODE_STRING   ModuleFileName,
	OUT PHANDLE           ModuleHandle
);

NTSTATUS WINAPI LdrUnloadDll(IN HANDLE ModuleHandle);
