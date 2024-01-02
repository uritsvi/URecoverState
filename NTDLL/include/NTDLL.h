#pragma once

#include <Windows.h>

typedef NTSTATUS(NTAPI* pNtSuspendProcess)(HANDLE ProcessHandle);

typedef NTSTATUS(NTAPI* pNtResumeProcess)(HANDLE ProcessHandle);

class NTDLL
{
public:

	static NTDLL& GetInstance();

	bool Init();

	NTSTATUS NTAPI NtSuspendProcess(_In_ HANDLE ProcessHandle);
	NTSTATUS NTAPI NtResumeProcess(_In_ HANDLE ProcessHandle);

private:
	pNtSuspendProcess m_NtSuspendProcess;
	pNtResumeProcess m_NtResumeProcess;
};

