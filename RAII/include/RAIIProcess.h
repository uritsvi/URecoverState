#pragma once

#include <Windows.h>
#include <string>
#include <memory>

class RAIIHandle;

#define _RAII_PROCESS 1

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
	std::shared_ptr<RAIIHandle> GetRAIIHandle();

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

	bool WriteMem(
		_In_ LPVOID TargetAddr, 
		_In_ LPVOID Buffer, 
		_In_ SIZE_T Size
	);
	bool ReadMem(
		_In_ LPVOID TargetAddr,
		_Out_ LPVOID Buffer,
		_In_ SIZE_T Size
	);
	bool WaitForExit(_Out_ DWORD& ExitCode);

	bool Terminate();

	LPVOID GetPEBAddr();

	void Close();

private:
	std::shared_ptr<RAIIHandle> m_Handle;
	DWORD m_PID;


};