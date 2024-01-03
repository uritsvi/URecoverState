#pragma once

#include <Windows.h>
#include <string>

class RAIIProcess {

public:
	RAIIProcess();
	~RAIIProcess();

	bool Create(
		_In_ std::string EXE, 
		_In_ std::string Cmd,
		_In_ ULONG Flags
	);
	bool Open(_In_ DWORD PID);

	DWORD GetPID();
	HANDLE GetHandle();

	bool FreeMem(_In_ LPVOID Addr);
	bool AllocMem(
		_In_ LPVOID Addr,
		_In_ SIZE_T Size,
		_In_ DWORD AllocType,
		_In_ DWORD Prot
	);
	bool VirtualProt(
		_In_ LPVOID Addr, 
		_In_ UINT Size, 
		_In_ DWORD Prot
	);

	bool Suspend();
	bool Resume();

	bool LoadDllToTargetProcess(
		_In_ std::string DllPath,
		_Out_ DWORD& Tid
	);

private:
	HANDLE m_Handle;
	DWORD m_PID;
};