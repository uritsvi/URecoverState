#include "..\RAII\include\RAIIProcess.h"
#include "include\TargetProcess.h"
#include "..\NTDLL\include\NTDLL.h"

TargetProcess& TargetProcess::GetInstance() {
	static TargetProcess target = TargetProcess();
	return target;

}

bool TargetProcess::Init(_In_ DWORD Pid) {
	bool res = true;

	do {
		m_Pid = Pid;

		m_Process = std::make_shared<RAIIProcess>();
		res = m_Process->Open(
			Pid
		);
		if (!res) {
			break;
		}
	} while (false);

	return res;
}
std::shared_ptr<RAIIProcess> TargetProcess::GetProcess() {
	return m_Process;
}

void TargetProcess::SetProcess(_In_ std::shared_ptr<RAIIProcess> Process) {
	m_Process = Process;
}


/*
bool TargetProcess::GetTargetThreadsList(_Out_ std::list<THREADENTRY32>& Threads) {
	const std::shared_ptr<void> hThreadSnapshot(
		CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
	
	if (hThreadSnapshot.get() == INVALID_HANDLE_VALUE) {
		ERROR_LOG("Failed to create a snapshot");
		return false;
	}
	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(THREADENTRY32);
	DWORD result = 0;
	for (BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
		!result && success && GetLastError() != ERROR_NO_MORE_FILES;
		success = Thread32Next(hThreadSnapshot.get(), &tEntry))
	{
		if (m_Pid == tEntry.th32OwnerProcessID) {
			Threads.push_back(tEntry);
		}
	}
	return true;
}

bool TargetProcess::FreeAllProcessPages() {
	bool res = true;
	

		MEMORY_BASIC_INFORMATION info;
		ULONG64 addr = 0;


		while (true) {
			res = VirtualQueryEx(
				m_Process, 
				(LPCVOID)addr, 
				&info, 
				sizeof(info)
			);
			if (!res) {
				ERROR_LOG("Failed to query virtual memory of process");
				break;
			}

			if (info.State == MEM_COMMIT && info.Protect == PAGE_READWRITE) {

				res = VirtualFreeEx(
					m_Process, 
					(LPVOID)info.BaseAddress,
					0,
					MEM_RELEASE
				);
				if (!res) {
					ERROR_LOG("Failed to free process virtual memory");
				}
			}

			addr = (ULONG64)info.BaseAddress + info.RegionSize;
		}

	return res;
}
*/

/*
bool TargetProcess::LoadDllToTargetProcess(
	_In_ std::string DllPath,
	_Out_ DWORD& Tid) {
	bool res = true;
	
	do {
		

		LPVOID dllParamAddr = VirtualAllocEx(
			m_Process,
			nullptr,
			DllPath.length(),
			MEM_COMMIT | MEM_RESERVE,
			PAGE_READWRITE
		);
		if (dllParamAddr == nullptr) {
			ERROR_LOG("Failed to allocate mem on target process");
			res = false;
			break;
		}

		res = WriteProcessMemory(
			m_Process, 
			dllParamAddr, 
			DllPath.c_str(), 
			DllPath.length(), 
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to write process target mem");
			break;
		}

		auto module = GetModuleHandleA("Kernel32.dll");
		if (!module) {
			ERROR_LOG("Failed to get handle to kernel32.dll");
			res = false;
			break;
		}

		FARPROC loadLibAddr = GetProcAddress(
			module, 
			"LoadLibraryA"
		);
		if (loadLibAddr == nullptr) {
			ERROR_LOG("Failed to get loadLibAddr addr");
			res = false;
			break;
		}

		HANDLE remoteThread = CreateRemoteThread(
			m_Process, 
			nullptr, 
			0, 
			(LPTHREAD_START_ROUTINE)loadLibAddr, 
			dllParamAddr, 
			0, 
			&Tid
		);
		if (remoteThread == nullptr) {
			res = false;
			ERROR_LOG("Failed to create remote thread");
			break;
		}

		DWORD exitCode;
		do {
			Sleep(500);
			GetExitCodeThread(remoteThread, &exitCode);
		
		} while (exitCode == STILL_ACTIVE);
	} while (false);

	return res;
}

bool TargetProcess::Suspend() {
	bool res = true;

	do {
		NTSTATUS status = 
			NTDLL::GetInstance().NtSuspendProcess(m_Process);
		if (status != CMC_STATUS_SUCCESS) {
			ERROR_LOG("Failed to suspend target process");
			res = false;
			break;
		}
	} while (false);	

	return res;
}
bool TargetProcess::Resume() {
	bool res = true;

	do {
		NTSTATUS status =
			NTDLL::GetInstance().NtResumeProcess(m_Process);
		if (status != CMC_STATUS_SUCCESS) {
			ERROR_LOG("Failed to resume target process");
			res = false;
			break;
		}
	} while (false);

	return res;
}
*/