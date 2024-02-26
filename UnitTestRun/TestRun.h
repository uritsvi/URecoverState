#pragma once

#include <Windows.h>
#include <DbgHelp.h>

#include "..\RAII\include\RAIIProcess.h"


#pragma comment(lib, "dbghelp.lib")

/*
class TestRun
{
public:
	void Run();
	bool DebuggerLoop();
	
private:
	bool HandleBreakpoint(_In_ EXCEPTION_DEBUG_INFO& Event);
	
	bool SetBpOnMain(_Out_ LPVOID& BpAddr);

	bool CompareSnapshots(_In_ const char* StateName);
	bool DisableBreakpoint(_In_ LPVOID Addr);


private:
};
*/

namespace TestRun {
	void Run();
	bool DebuggerLoop();

	bool HandleBreakpoint(_In_ EXCEPTION_DEBUG_INFO& Event);

	bool SetBpOnMain(_Out_ LPVOID& BpAddr);

	bool CompareSnapshots(_In_ const char* StateName);
	bool DisableBreakpoint(_In_ LPVOID Addr);

}

