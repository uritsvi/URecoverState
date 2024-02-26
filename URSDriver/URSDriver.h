#pragma once

#include <Windows.h>

#include "NTOSKRNLSymbols.h"
#include "../Common/include/Common.h"


class URSDriver
{
public:
	bool Init();

	bool LoadKernelSymbols();
	bool MoveThreadId(
		_In_ DWORD Target,
		_In_ DWORD Src
	);

	//bool FourceResumeThread(_In_ DWORD Tid);
	bool ChangeThreadTEB(
		_In_ DWORD Tid, 
		_In_ LPVOID NewTEB);

	bool ChangeProcessId(
		_In_ DWORD Pid, 
		_In_ DWORD TargetId
	);

	bool ChangeHandle(
		_In_ HANDLE SrcHandle,
		_In_ HANDLE TargetHandle,
		_In_ DWORD Pid
	);

private:
	bool OpenDriver();
	bool LoadDriver();

	bool GetBaseNTOSKRNL(_Out_ KernelInfo& Addr);
	bool PassSymbolsToDriver(_In_ KernelFunctions& Symbols);


private:
	HANDLE m_DriverHandle;
	NTOSKRNLSymbols m_Symbols;
};

