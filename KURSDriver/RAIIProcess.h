#pragma once

#include <ntddk.h>

class RAIIProcess
{
public:
	RAIIProcess();
	~RAIIProcess();

	NTSTATUS Open(HANDLE Id);

	PVOID GetEProcess();

	HANDLE GetId();

private:
	PEPROCESS m_Process;
	HANDLE m_Id;
};

