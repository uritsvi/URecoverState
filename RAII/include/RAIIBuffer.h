#pragma once 

#include <Windows.h>

class RAIIBuffer {

public:
	RAIIBuffer();
	~RAIIBuffer();

	bool Create(ULONG64 Size);

	LPVOID GetPtr();
	ULONG64 GetSize();

private:
	LPVOID m_Buffer;
	ULONG64 m_Size;
};