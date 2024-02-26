#pragma once

#include <ntddk.h>


class RAIIThread
{

public:
	RAIIThread();
	~RAIIThread();

	NTSTATUS Open(HANDLE Id);

	PVOID GetEthread();

	HANDLE GetId();

private:
	PVOID m_Thread;
	HANDLE m_Id;
};

