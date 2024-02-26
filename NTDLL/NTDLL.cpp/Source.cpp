#include "..\include\NTDLL.h"

NTDLL& NTDLL::GetInstance() {

	static NTDLL ntdll;
	return ntdll;
}

bool NTDLL::Init() {
	bool res = true;
	do {
		HMODULE ntdllHandle = GetModuleHandleA("ntdll.dll");

		if (ntdllHandle == nullptr) {
			res = false;
			break;
		}
		m_NtSuspendProcess =
			(pNtSuspendProcess)GetProcAddress(ntdllHandle, "NtSuspendProcess");
		if (m_NtSuspendProcess == nullptr) {
			res = false;
			break;
		}

		m_NtResumeProcess =
			(pNtResumeProcess)GetProcAddress(ntdllHandle, "NtResumeProcess");
		if (m_NtResumeProcess == nullptr) {
			res = false;
			break;
		}

		m_NtQuerySystemInformation = 
			(pNtQuerySystemInformation)GetProcAddress(ntdllHandle, "NtQuerySystemInformation");
		if (m_NtQuerySystemInformation == nullptr) {
			res = false;
			break;
		}

		m_RtlCloneUserProcess = 
			(pRtlCloneUserProcess)GetProcAddress(ntdllHandle, "RtlCloneUserProcess");
		if (m_RtlCloneUserProcess == nullptr) {
			res = false;
			break;
		}

		m_NtQueryInformationThread = 
			(pNtQueryInformationThread)GetProcAddress(ntdllHandle, "NtQueryInformationThread");
		if (m_NtQueryInformationThread == nullptr) {
			res = false;
			break;
		}

		m_NtSetInformationProcess = 
			(pNtSetInformationProcess)GetProcAddress(ntdllHandle, "NtSetInformationProcess");
		if (m_NtSetInformationProcess == nullptr) {
			res = false;
			break;
		}

		m_NtQueryInformationProcess =
			(pNtQueryInformationProcess)GetProcAddress(ntdllHandle, "NtQueryInformationProcess");

	} while (false);

	return res;
}

NTSTATUS NTAPI NTDLL::NtSuspendProcess(_In_ HANDLE ProcessHandle) {
	return m_NtSuspendProcess(ProcessHandle);
}

NTSTATUS NTAPI NTDLL::NtResumeProcess(_In_ HANDLE ProcessHandle) {
	return m_NtResumeProcess(ProcessHandle);
}

NTSTATUS NTAPI NTDLL::NtQuerySystemInformation(
	_In_		  UINT				       SystemInformationClass,
	_Inout_		  PVOID                    SystemInformation,
	_In_          ULONG                    SystemInformationLength,
	_Inout_       PULONG                   ReturnLength) {

	return m_NtQuerySystemInformation(
		SystemInformationClass, 
		SystemInformation, 
		SystemInformationLength, 
		ReturnLength
	);
}

NTSTATUS NTAPI NTDLL::RtlCloneUserProcess(
	ULONG ProcessFlags,
	PSECURITY_DESCRIPTOR ProcessSecurityDescriptor,
	PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
	HANDLE DebugPort,
	PRTL_USER_PROCESS_INFORMATION ProcessInformation) {

	return m_RtlCloneUserProcess(
		ProcessFlags,
		ProcessSecurityDescriptor, 
		ThreadSecurityDescriptor, 
		DebugPort, 
		ProcessInformation
	);
}

NTSTATUS NTAPI NTDLL::NtQueryInformationThread(
	HANDLE          ThreadHandle,
	THREADINFOCLASS ThreadInformationClass,
	PVOID           ThreadInformation,
	ULONG           ThreadInformationLength,
	PULONG          ReturnLength) {

	return m_NtQueryInformationThread(
		ThreadHandle, 
		ThreadInformationClass, 
		ThreadInformation, 
		ThreadInformationLength, 
		ReturnLength
	);
}

NTSTATUS NTAPI NTDLL::NtSetInformationProcess(
	HANDLE Process,
	PROCESS_INFORMATION_CLASS InfoClass,
	PVOID Buffer,
	ULONG Size) {

	return m_NtSetInformationProcess(
		Process,
		InfoClass,
		Buffer,
		Size
	);
}

NTSTATUS NTAPI NTDLL::NtQueryInformationProcess(
	HANDLE           ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID            ProcessInformation,
	ULONG            ProcessInformationLength,
	PULONG           ReturnLength) {

	return m_NtQueryInformationProcess(
		ProcessHandle, 
		ProcessInformationClass, 
		ProcessInformation, 
		ProcessInformationLength, 
		ReturnLength
	);
}