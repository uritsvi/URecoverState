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
	} while (false);

	return res;
}

NTSTATUS NTAPI NTDLL::NtSuspendProcess(_In_ HANDLE ProcessHandle) {
	return m_NtSuspendProcess(ProcessHandle);
}

NTSTATUS NTAPI NTDLL::NtResumeProcess(_In_ HANDLE ProcessHandle) {
	return m_NtResumeProcess(ProcessHandle);
}
