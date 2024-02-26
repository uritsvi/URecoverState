#include "include/RAIIProcess.h"
#include "..\NTDLL\include\NTDLL.h"
#include <TlHelp32.h>
#include <iostream>
#include <tlhelp32.h>

#include "..\Common\include\Error.h"
#include "include/RAIIThread.h"
#include "include\RAIIHandle.h"

#pragma comment(lib, "NTDLL.lib")


RAIIProcess::RAIIProcess() {
	m_PID = 0;
}
RAIIProcess::~RAIIProcess() {
	Close();
}

void RAIIProcess::Close() {
	std::cout << "Close: " << m_PID << " Line: " << __LINE__ << "\n";
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

		m_Handle = std::make_shared<RAIIHandle>(processInfo.hProcess);
		m_PID = processInfo.dwProcessId;

		CloseHandle(processInfo.hThread);

		std::cout << "Opn: " << m_PID << " Line: " << __LINE__ << "\n";


	} while (false);

	return res;
}
bool RAIIProcess::Open(_In_ DWORD PID) {
	bool res = true;

	std::cout << "Opn: " << PID << " Line: " << __LINE__ << "\n";


	do {
		m_PID = PID;
		HANDLE handle = OpenProcess(
			PROCESS_ALL_ACCESS, 
			FALSE, 
			PID
		);
		if (handle == nullptr) {
			res = false;
			break;
		}
		m_Handle = std::make_shared<RAIIHandle>(handle);

	} while (false);

	return res;
}

DWORD RAIIProcess::GetPID() {
	return m_PID;
}

std::shared_ptr<RAIIHandle> RAIIProcess::GetRAIIHandle() {
	return m_Handle;
}

bool RAIIProcess::FreeMem(_In_ LPVOID Addr) {
	return VirtualFreeEx(
		m_Handle->GetHandle(),
		Addr, 
		0, 
		MEM_RELEASE
	);
}

bool RAIIProcess::AllocMem(
	_In_ LPVOID Addr,
	_In_ SIZE_T Size,
	_In_ DWORD AllocType,
	_In_ DWORD Prot) {

	return VirtualAllocEx(
		m_Handle->GetHandle(),
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
		m_Handle->GetHandle(),
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
			NTDLL::GetInstance().NtSuspendProcess(m_Handle->GetHandle());
		if (status != CMC_STATUS_SUCCESS) {
			res = false;
			break;
		}

		/*
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (h != INVALID_HANDLE_VALUE) {
			THREADENTRY32 te;
			te.dwSize = sizeof(te);
			if (Thread32First(h, &te)) {
				do {

					if (m_PID != te.th32OwnerProcessID) {
						continue;
					}

						RAIIThread thread;
						thread.Open(te.th32ThreadID);

						LPVOID startAddr = 0;
						status = NTDLL::GetInstance().NtQueryInformationThread(
							thread.GetHandle(), 
							(THREADINFOCLASS)ThreadQuerySetWin32StartAddress, 
							&startAddr, 
							8, 
							nullptr
						);

						auto a = GetModuleHandleA("ntdll.dll");
						FARPROC pTppWorkerThread = GetProcAddress(
							a, 
							"TppWorkerThread"
						);

						if (pTppWorkerThread == startAddr) {
							thread.Resume();
						}


					
					te.dwSize = sizeof(te);
				} while (Thread32Next(h, &te));
			}
			CloseHandle(h);
		}
		return 0;
		*/
	} while (false);

	return res;
	

	/*
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		return false;
	}

	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32)) {
		CloseHandle(hThreadSnap);
		return false;
	}

	int c = 0;
	do {
		if (te32.th32OwnerProcessID == GetProcessIdOfThread(m_Handle)) {
			//c = SuspendThread(m_Handle);
		
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	*/
}
bool RAIIProcess::Resume() {
	
	bool res = true;


	do {
		NTSTATUS status =
			NTDLL::GetInstance().NtResumeProcess(m_Handle->GetHandle());
		if (status != CMC_STATUS_SUCCESS) {
			res = false;
			break;
		}
	} while (false);

	return res;
	
	/*
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		return false;
	}

	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32)) {
		CloseHandle(hThreadSnap);
		return false;
	}

	int c = 0;
	do {
		if (te32.th32OwnerProcessID == GetProcessIdOfThread(m_Handle)) {
			c = ResumeThread(m_Handle);
		}
	} while (Thread32Next(hThreadSnap, &te32));

	std::cout << "Susspend" << c << "\n";
	CloseHandle(hThreadSnap);
	*/
}

bool RAIIProcess::LoadDllToTargetProcess(
	_In_ std::string DllPath,
	_Out_ DWORD& Tid) {
	bool res = true;

	do {


		LPVOID dllParamAddr = VirtualAllocEx(
			m_Handle->GetHandle(),
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
			m_Handle->GetHandle(),
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
			m_Handle->GetHandle(),
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
			res = GetExitCodeThread(remoteThread, &exitCode);

		} while (exitCode == STILL_ACTIVE);

		res = CloseHandle(remoteThread);
		

	} while (false);

	return res;
}

bool RAIIProcess::WriteMem(
	_In_ LPVOID TargetAddr,
	_In_ LPVOID Buffer,
	_In_ SIZE_T Size) {

	return WriteProcessMemory(
		m_Handle->GetHandle(),
		TargetAddr, 
		Buffer, 
		Size, 
		nullptr
	);
}

bool RAIIProcess::ReadMem(
	_In_ LPVOID TargetAddr,
	_Out_ LPVOID Buffer,
	_In_ SIZE_T Size) {

	return ReadProcessMemory(
		m_Handle->GetHandle(),
		TargetAddr, 
		Buffer, 
		Size, 
		nullptr
	);
}

bool RAIIProcess::WaitForExit(_Out_ DWORD& ExitCode) {
	bool res = true;
	
	do {
		Sleep(500);
		
		res = GetExitCodeProcess(
			m_Handle->GetHandle(),
			&ExitCode
		);
		if (!res || ExitCode != STILL_ACTIVE) {
			break;
		}

	} while (true);

	return res;
}

LPVOID RAIIProcess::GetPEBAddr() {
	PROCESS_BASIC_INFORMATION info;
	NTDLL::GetInstance().NtQueryInformationProcess(
		m_Handle->GetHandle(),
		ProcessBasicInformation, 
		&info ,
		sizeof(info), 
		nullptr
	);
	return info.PebBaseAddress;

}

bool RAIIProcess::Terminate(){
	return TerminateProcess(
		m_Handle->GetHandle(), 
		0
	);
}