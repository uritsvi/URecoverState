#pragma once

#include <windows.h>
#include <vector>
#include <string>

class CSVBuffer {

public:
	void PushRow(_In_ std::string Row);

	/*
	* Returns the address of the buffer affter writing
	*/
	/*
	LPVOID Serialize(_Out_ LPVOID Buffer);
	LPVOID Load(_In_ LPVOID Buffer);
	*/
	ULONG64 GetSerializedSize();

	bool GetByIndex(
		_In_ ULONG Coloumn,
		_In_ ULONG Row,
		_In_ std::string& Str
	);

private:
	ULONG64 m_Size;
	std::vector<std::vector<std::string>> m_Buffers;
};

