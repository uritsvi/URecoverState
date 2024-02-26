#include "include\URSDriverInterface.h"
#include "URSDriver.h"

#pragma comment(lib, "RAII.lib")

URSDriver g_URSDriver;

bool URSDriverInit() {
	return g_URSDriver.Init();
}

bool URSDriverMoveThreadId(
	_In_ DWORD Target,
	_In_ DWORD Src) {

	return g_URSDriver.MoveThreadId(
		Target, 
		Src
	);
}

bool URSDriverChangeThreadTEB(_In_ DWORD Tid, _In_ LPVOID NewTEB) {
	return g_URSDriver.ChangeThreadTEB(
		Tid, 
		NewTEB
	);
}

bool URSDriverChangeProcessId(
	_In_ DWORD Pid,
	_In_ DWORD TargetId) {

	return g_URSDriver.ChangeProcessId(
		Pid,
		TargetId
	);
}

bool URSDriverChangeHandle(
	_In_ HANDLE SrcHandle,
	_In_ HANDLE TargetHandle,
	_In_ DWORD Pid)
{
	return g_URSDriver.ChangeHandle(
		SrcHandle,
		TargetHandle,
		Pid
	);
}