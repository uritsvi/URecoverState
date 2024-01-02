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

private:
	HANDLE m_Handle;
	DWORD m_PID;
};