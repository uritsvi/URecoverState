#pragma once 

#include <Windows.h>

class RAIIBuffer {

public:
	RAIIBuffer() = default;
	RAIIBuffer(ULONG64 Size);
	~RAIIBuffer();

	LPVOID GetPtr();
	ULONG64 GetSize();

private:
	LPVOID m_Buffer;
	ULONG64 m_Size;
};