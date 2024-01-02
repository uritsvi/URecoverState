#pragma once

#include <Windows.h>
#include <DbgHelp.h>

#include "..\RAII\include\RAIIProcess.h"


#pragma comment(lib, "dbghelp.lib")

class TestRun
{
public:
	void Run();
	bool DebuggerLoop(_In_ RAIIProcess& Process);
	
	bool HandleBreakpoint(DEBUG_EVENT& Event);
};

