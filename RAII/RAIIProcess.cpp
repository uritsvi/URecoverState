#include "include/RAIIProcess.h"
#include "..\NTDLL\include\NTDLL.h"

RAIIProcess::RAIIProcess() {
	m_Handle = nullptr;
	m_PID = 0;
}
RAIIProcess::~RAIIProcess() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

bool RAIIProcess::Create(
	_In_ std::string EXE,
	_In_ std::string Cmd,
	_In_ ULONG Flags) {

	bool res = true;

	do {
		std::string cmd = EXE + " " + Cmd;

		STARTUPINFOA info = {0};
		info.cb = sizeof(info);

		PROCESS_INFORMATION processInfo = {0};

		res = CreateProcessA(
			EXE.c_str(),
			(LPSTR)cmd.c_str(),
			nullptr,
			nullptr,
			FALSE,
			Flags,
			nullptr,
			nullptr, 
			&info,
			&processInfo
		);
		if (!res) {
			break;
		}

		m_Handle = processInfo.hProcess;
		m_PID = processInfo.dwProcessId;
	} while (false);

	return res;
}
bool RAIIProcess::Open(_In_ DWORD PID) {
	bool res = true;

	do {
		m_PID;
		m_Handle = OpenProcess(
			PROCESS_ALL_ACCESS, 
			FALSE, 
			PID
		);
		if (m_Handle == nullptr) {
			res = false;
			break;
		}

	} while (false);

	return res;
}

DWORD RAIIProcess::GetPID() {
	return m_PID;
}

HANDLE RAIIProcess::GetHandle() {
	return m_Handle;
}

bool RAIIProcess::FreeMem(_In_ LPVOID Addr) {
	return VirtualFreeEx(
		m_Handle, 
		Addr, 
		0, 
		MEM_DECOMMIT
	);
}

bool RAIIProcess::AllocMem(
	_In_ LPVOID Addr,
	_In_ SIZE_T Size,
	_In_ DWORD AllocType,
	_In_ DWORD Prot) {

	return VirtualAllocEx(
		m_Handle, 
		Addr, 
		Size, 
		AllocType, 
		Prot
	);
}

bool RAIIProcess::VirtualProt(
	_In_ LPVOID Addr,
	_In_ UINT Size,
	_In_ DWORD Prot) {

	DWORD oldProt;
	return VirtualProtectEx(
		m_Handle, 
		Addr, 
		Size, 
		Prot, 
		&oldProt
	);
}

bool RAIIProcess::Suspend() {
	bool res = true;

	do {
		NTSTATUS status =
			NTDLL::GetInstance().NtSuspendProcess(m_Handle);
		if (status != CMC_STATUS_SUCCESS) {
			res = false;
			break;
		}
	} while (false);

	return res;
}
bool RAIIProcess::Resume() {
	bool res = true;

	do {
		NTSTATUS status =
			NTDLL::GetInstance().NtResumeProcess(m_Handle);
		if (status != CMC_STATUS_SUCCESS) {
			res = false;
			break;
		}
	} while (false);

	return res;
}

bool RAIIProcess::LoadDllToTargetProcess(
	_In_ std::string DllPath,
	_Out_ DWORD& Tid) {
	bool res = true;

	do {


		LPVOID dllParamAddr = VirtualAllocEx(
			m_Handle,
			nullptr,
			DllPath.length(),
			MEM_COMMIT | MEM_RESERVE,
			PAGE_READWRITE
		);
		if (dllParamAddr == nullptr) {
			res = false;
			break;
		}

		res = WriteProcessMemory(
			m_Handle,
			dllParamAddr,
			DllPath.c_str(),
			DllPath.length(),
			nullptr
		);
		if (!res) {
			break;
		}

		auto module = GetModuleHandleA("Kernel32.dll");
		if (!module) {
			res = false;
			break;
		}

		FARPROC loadLibAddr = GetProcAddress(
			module,
			"LoadLibraryA"
		);
		if (loadLibAddr == nullptr) {
			res = false;
			break;
		}

		HANDLE remoteThread = CreateRemoteThread(
			m_Handle,
			nullptr,
			0,
			(LPTHREAD_START_ROUTINE)loadLibAddr,
			dllParamAddr,
			0,
			&Tid
		);
		if (remoteThread == nullptr) {
			res = false;
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