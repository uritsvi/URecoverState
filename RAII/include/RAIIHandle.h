#pragma once

#include <Windows.h>
#include <memory>

class RAIIProcess;



class RAIIHandle
{
public:
	RAIIHandle();
	RAIIHandle(HANDLE Handle);

	~RAIIHandle();

	HANDLE GetHandle();
	bool DupHandle(
		_In_ std::shared_ptr<RAIIProcess> Src, 
		_In_ std::shared_ptr<RAIIProcess> Target,
		_Out_ HANDLE& Out
	);

private:
	HANDLE m_Handle;
};

