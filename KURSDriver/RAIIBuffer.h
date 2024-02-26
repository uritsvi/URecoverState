#pragma once

#include <ntddk.h>

class RAIIBuffer

{
public:
	RAIIBuffer();
	RAIIBuffer(_In_ SIZE_T Size);
	
	~RAIIBuffer();

	PVOID64 GetAddr();
	
	NTSTATUS Move(_In_ RAIIBuffer& Buffer);

private:
	PVOID64 m_Buffer;
	bool m_BufferOwner;
};

