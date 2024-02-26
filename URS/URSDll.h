#pragma once

#include <Windows.h>
#include <string>

#include "..\Common/include/SharedMemory.h"
#include "..\RAII\include\RAIIThread.h"

#pragma comment(lib, "Common.lib")

class URSDLL
{
public:
	static URSDLL& GetInstance();

	bool IsLoaded();
	bool Load();

	//void GetDllThreadId(DWORD& Tid);

	bool CloneTargetProcess(_Out_ DWORD& Pid);

private:

	bool FinishDllInit();

	bool CallDllFunction(
		_In_ LPARAM LParam,
		_In_ UINT Function,
		_Out_ UINT& ReturnValue
	);

	bool InitSharedMemory();

private:	
	DWORD m_Tid;
	bool m_IsLoaded;

	SharedMemory m_SharedMemory;
};
