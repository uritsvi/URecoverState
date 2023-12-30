#pragma once

#include <Windows.h>

class RAIIThread
{
public:
	RAIIThread();
	~RAIIThread();

	bool Open(_In_ DWORD Tid);
	bool Create(
		_In_ LPTHREAD_START_ROUTINE Function, 
		_In_ LPVOID Ctx,
		_Out_ DWORD& Tid
	);
	
	bool SetContext(_In_ PCONTEXT Context);

	bool Resume();

	bool Terminate();

private:
	HANDLE m_Handle;
};

