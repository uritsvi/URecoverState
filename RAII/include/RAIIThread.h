#pragma once

#include <Windows.h>
#include <memory>

class RAIIHandle;
class RAIIProcess;

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
	bool Create(
		_In_ std::shared_ptr<RAIIProcess> Process,
		_In_ LPVOID Addr, 
		_In_ LPVOID Ctx,
		_In_ DWORD Flags,
		_Out_ DWORD& Tid
	);

	

	bool GetContext(_In_ PCONTEXT Context);
	bool SetContext(_In_ PCONTEXT Context);

	bool Suspend();
	bool Resume();

	bool Terminate();

	bool WaitForTerminate(_Out_ DWORD& Code, DWORD TimeOutInSeconds = INFINITE);

	DWORD GetTid();

	std::shared_ptr<RAIIHandle> GetRAIIHandle();
	
	/*
	* Calls resume thread on the thread until count == 0
	*/
	bool FourceResume();

	bool GetThreadSuspendCount(_Out_ DWORD& C);
	bool SetSuspendCount(_In_ DWORD C);

	LPVOID GetTEB();

private:
	void SetThreadDesc();

private:
	std::shared_ptr<RAIIHandle> m_Handle;
	DWORD m_Tid;
};

