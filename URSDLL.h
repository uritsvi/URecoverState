#pragma once

#include <Windows.h>
#include <string>

#include "Common/include/SharedMemory.h"

#pragma comment(lib, "Common.lib")

class URSDLL
{
public:
	bool IsLoaded();
	bool Load();

	bool MapFileToTarget(
		_In_ std::string Path, 
		_In_ UINT64 Addr
	);
	bool UnmapFileFromTarget(_In_ UINT64 Addr);
	
	bool ResumeDllThread();
private:

	bool FinishDllInit();

	bool CallDllFunction(
		_In_ LPARAM LParam,
		_In_ UINT Function, 
		_Out_ UINT& ReturnValue
	);

	bool InitSharedMemory();

private:
	DWORD m_RemoteThreadTid;
	bool m_IsLoaded;

	SharedMemory m_SharedMemory;
};

