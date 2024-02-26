#pragma once

#include <Windows.h>

#include <array>

#include "..\Common\include\Common.h"
#include "..\RAII\include\RAIIThread.h"
#include "..\Common\include\SharedMemory.h"
#include "DllFunctions.h"

#pragma comment(lib, "Dbghelp.lib")


class URSDLLServer
{
public:
	bool Init();
	DWORD SelfDllRun();

private:
	
	bool RegisterDllFunction(
		_In_ UINT Index, 
		_In_ DllFunction Function
	);

	bool ReturnOp(
		_In_ bool Res, 
		_In_ LPARAM LParam,
		_In_ DWORD TargetTid=0
	);

	bool HandleFunctionCallOp(
		_In_ WPARAM WParam, 
		_In_ LPARAM LParam
	);

	bool InitSharedMemory(_In_ DWORD ThreadId);

	bool RegisterDllFunctions();


private:
	LPVOID m_SharedMemoryBaseAddr;
	RAIIThread m_RunThread;

	std::array<DllFunction, MAX_DLL_FUNCTIONS> m_FunctionsTable;

	std::shared_ptr<SharedMemory> m_SharedMemory;
	DllFunctions m_DllFunctions;
};

