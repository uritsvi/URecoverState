#pragma once

#include <Windows.h>
#include <winternl.h>

#define SystemHandleInformation 0x10

typedef NTSTATUS(NTAPI* pNtSuspendProcess)(HANDLE ProcessHandle);

typedef NTSTATUS(NTAPI* pNtResumeProcess)(HANDLE ProcessHandle);

typedef NTSTATUS (NTAPI* pNtQuerySystemInformation)(
	_In_          UINT SystemInformationClass,
	_Inout_       PVOID                    SystemInformation,
	_In_          ULONG                    SystemInformationLength,
	_Inout_       PULONG                   ReturnLength
);

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

// handle table information
typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef struct _SECTION_IMAGE_INFORMATION
{
	PVOID TransferAddress;
	ULONG ZeroBits;
	SIZE_T MaximumStackSize;
	SIZE_T CommittedStackSize;
	ULONG SubSystemType;
	union
	{
		struct
		{
			USHORT SubSystemMinorVersion;
			USHORT SubSystemMajorVersion;
		};
		ULONG SubSystemVersion;
	};
	union
	{
		struct
		{
			USHORT MajorOperatingSystemVersion;
			USHORT MinorOperatingSystemVersion;
		};
		ULONG OperatingSystemVersion;
	};
	USHORT ImageCharacteristics;
	USHORT DllCharacteristics;
	USHORT Machine;
	BOOLEAN ImageContainsCode;
	union
	{
		UCHAR ImageFlags;
		struct
		{
			UCHAR ComPlusNativeReady : 1;
			UCHAR ComPlusILOnly : 1;
			UCHAR ImageDynamicallyRelocated : 1;
			UCHAR ImageMappedFlat : 1;
			UCHAR BaseBelow4gb : 1;
			UCHAR ComPlusPrefer32bit : 1;
			UCHAR Reserved : 2;
		};
	};
	ULONG LoaderFlags;
	ULONG ImageFileSize;
	ULONG CheckSum;
} SECTION_IMAGE_INFORMATION, * PSECTION_IMAGE_INFORMATION;

typedef struct _RTL_USER_PROCESS_INFORMATION
{
	ULONG Length;
	HANDLE ProcessHandle;
	HANDLE ThreadHandle;
	CLIENT_ID ClientId;
	SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION, * PRTL_USER_PROCESS_INFORMATION;

#define RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED 0x00000001
#define RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES 0x00000002
#define RTL_CLONE_PROCESS_FLAGS_NO_SYNCHRONIZE 0x00000004

#define RTL_CLONE_PARENT				0
#define RTL_CLONE_CHILD					297

typedef NTSTATUS (*pRtlCloneUserProcess)(
	ULONG ProcessFlags,
	PSECURITY_DESCRIPTOR ProcessSecurityDescriptor /* optional */,
	PSECURITY_DESCRIPTOR ThreadSecurityDescriptor /* optional */,
	HANDLE DebugPort /* optional */,
	PRTL_USER_PROCESS_INFORMATION ProcessInformation);


typedef NTSTATUS (*pNtQueryInformationThread)(
	HANDLE          ThreadHandle,
	THREADINFOCLASS ThreadInformationClass,
	PVOID           ThreadInformation,
	ULONG           ThreadInformationLength,
	PULONG          ReturnLength
);

typedef NTSTATUS (*pNtSetInformationProcess)(
	HANDLE Process,
	PROCESS_INFORMATION_CLASS InfoClass,
	PVOID Buffer,
	ULONG Size
);

typedef NTSTATUS (*pNtQueryInformationProcess)(
	HANDLE           ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID            ProcessInformation,
	ULONG            ProcessInformationLength,
	PULONG           ReturnLength
);


#define ThreadQuerySetWin32StartAddress 9

class NTDLL
{
public:

	static NTDLL& GetInstance();

	bool Init();

	NTSTATUS NTAPI NtSuspendProcess(_In_ HANDLE ProcessHandle);
	NTSTATUS NTAPI NtResumeProcess(_In_ HANDLE ProcessHandle);
	NTSTATUS NTAPI NtQuerySystemInformation(
		_In_		  UINT SystemInformationClass,
		_Inout_		  PVOID                    SystemInformation,
		_In_          ULONG                    SystemInformationLength,
		_Inout_       PULONG                   ReturnLength
	);
	NTSTATUS RtlCloneUserProcess(
		ULONG ProcessFlags,
		PSECURITY_DESCRIPTOR ProcessSecurityDescriptor /* optional */,
		PSECURITY_DESCRIPTOR ThreadSecurityDescriptor /* optional */,
		HANDLE DebugPort /* optional */,
		PRTL_USER_PROCESS_INFORMATION ProcessInformation);

	NTSTATUS NtQueryInformationThread(
		HANDLE          ThreadHandle,
		THREADINFOCLASS ThreadInformationClass,
		PVOID           ThreadInformation,
		ULONG           ThreadInformationLength,
		PULONG          ReturnLength
		);

	NTSTATUS NtSetInformationProcess(
		HANDLE Process, 
		PROCESS_INFORMATION_CLASS InfoClass, 
		PVOID Buffer, 
		ULONG Size
	);
	NTSTATUS NtQueryInformationProcess(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength,
		PULONG           ReturnLength
	);


private:
	pNtSuspendProcess m_NtSuspendProcess;
	pNtResumeProcess m_NtResumeProcess;
	pNtQuerySystemInformation m_NtQuerySystemInformation;
	pRtlCloneUserProcess m_RtlCloneUserProcess;
	pNtQueryInformationThread m_NtQueryInformationThread;
	pNtSetInformationProcess m_NtSetInformationProcess;
	pNtQueryInformationProcess m_NtQueryInformationProcess;
};

