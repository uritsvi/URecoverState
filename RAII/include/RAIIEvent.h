#pragma once

#include <Windows.h>

class RAIIEvent {

public:
	RAIIEvent();
	~RAIIEvent();

	bool Create(bool ManualReset=true);

	bool Set();
	bool Wait(_In_ DWORD TimeOut = INFINITE);
	bool Reset();

	bool IsSet();

private:
	HANDLE m_Handle;
};