#pragma once

#include <Windows.h>

/*
*	A class that is in use by the dll and the program 
*	that provides a way of comunication
*/
class SharedMemory
{
public:
	bool Create();
	bool Open();

	void SetTargetTid(_In_ DWORD PID);


	bool Read(
		_In_ ULONG64 Offset, 
		_In_ ULONG64 Size, 
		_Out_ char* Buffer
	);
	bool Write(
		_In_ ULONG64 Offset,
		_In_ ULONG64 Size,
		_In_ char* Buffer
	);

	bool ReadStr(
		_In_ ULONG64 MaxSize,
		_In_ ULONG64 Offset,
		_Out_ char* Buffer,
		_Out_ ULONG64& Len
	);
	bool WriteStr(
		_In_ const char* Buffer, 
		_In_ ULONG64 Offset
	);

	bool SendMsg(
		_In_ UINT Msg,
		_In_ WPARAM WParam,
		_In_ LPARAM LParam);
	
	bool ReciveMsg(
		_Out_ UINT& Msg,
		_Out_ WPARAM& WParam,
		_Out_ LPARAM& LParam);

	bool ReciveMsg(_Out_ UINT& Msg);

private:
	LPVOID m_SharedBufferAddr;
	DWORD m_TargetClientPID;
};

