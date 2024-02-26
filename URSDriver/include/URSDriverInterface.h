#pragma once 

#include <Windows.h>

bool URSDriverInit();
bool URSDriverMoveThreadId(
	_In_ DWORD Target,
	_In_ DWORD Src
);
bool URSDriverChangeThreadTEB(_In_ DWORD Tid, _In_ LPVOID NewTEB);

bool URSDriverChangeProcessId(_In_ DWORD Pid, _In_ DWORD TargetId);

bool URSDriverChangeHandle(
	_In_ HANDLE SrcHandle, 
	_In_ HANDLE TargetHandle, 
	_In_ DWORD Pid
);