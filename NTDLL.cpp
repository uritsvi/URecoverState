#include "NTDLL.h"
#include "Error.h"


NTDLL& NTDLL::GetInstance() {
	static NTDLL ntdll;
	return ntdll;
}

bool NTDLL::Init() {
	bool res = true;
	do {
		HMODULE ntdllHandle = GetModuleHandleA("ntdll.dll");
		
		if (ntdllHandle == nullptr) {
			ERROR_LOG("Failed to get ntdll handle");
			res = false;
			break;
		}
		m_NtSuspendProcess = 
			(pNtSuspendProcess)GetProcAddress(ntdllHandle, "NtSuspendProcess");
		if (m_NtSuspendProcess == nullptr) {
			ERROR_LOG("Failed to find NtSuspendProcess");
			res = false;
			break;
		}

		m_NtResumeProcess =
			(pNtResumeProcess)GetProcAddress(ntdllHandle, "NtResumeProcess");
		if (m_NtResumeProcess == nullptr) {
			ERROR_LOG("Failed to resume target process");
			res = false;
			break;
		}
	} while (false);

	return res;
}

NTSTATUS NTAPI NTDLL::NtSuspendProcess(_In_ HANDLE ProcessHandle) {
	return m_NtSuspendProcess(ProcessHandle);
}

NTSTATUS NTAPI NTDLL::NtResumeProcess(_In_ HANDLE ProcessHandle) {
	return m_NtResumeProcess(ProcessHandle);
}
