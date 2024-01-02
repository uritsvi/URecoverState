#include "TestRun.h"
#include "..\Error.h"
#include "..\Common.h"
#include "..\include\URSInterface.h"
#include "..\NTDLL\include\NTDLL.h"

#pragma comment(lib, "URS.lib")
#pragma comment(lib, "RAII.lib")
#pragma comment(lib, "NTDLL.lib")

#define TESTS_PROCESS_NAME "Tests.exe"

RAIIProcess g_TargetProcess;
bool g_FirstBP = true;

void TestRun::Run() {
	bool res = true;

	do {
		res = NTDLL::GetInstance().Init();
		if (!res) {
			ERROR_LOG("Failed to init ntdll");
			break;
		}

		res = g_TargetProcess.Create(
			TESTS_PROCESS_NAME,
			"",
			DEBUG_ONLY_THIS_PROCESS
		);
		if (!res) {
			ERROR_LOG("Failed to create target process");
			break;
		}
		
		res = InitURS(g_TargetProcess.GetPID());
		if (!res) {
			ERROR_LOG("Failed to init urs");
			break;
		}

		res = DebuggerLoop(g_TargetProcess);
		if (!res) {
			ERROR_LOG("Failed to run debugger loop");
			break;
		}

	} while (false);
		
	if (!res) {
		ERROR_LOG("Failed to run all tests");
	}
}

bool TestRun::DebuggerLoop(_In_ RAIIProcess& Process) {
	bool res = true;
	NTSTATUS status = 0;

	DEBUG_EVENT event;

	while (true) {
		res = WaitForDebugEvent(
			&event, 
			INFINITE
		);
		if (!res) {
			ERROR_LOG("Failed to wait for dbg events");
			break;
		}

		switch (event.dwDebugEventCode) {
			case EXCEPTION_DEBUG_EVENT: {
				auto ex = event.u.Exception;
				if (ex.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT) {
					break;
				}
				if (g_FirstBP) {
					g_FirstBP = false;
					break;
				}
				res = HandleBreakpoint(event);
				if (!res) {
					ERROR_LOG("Failed to handle breakpoint");
					break;
				}

			}break;
		}

		res = ContinueDebugEvent(
			event.dwProcessId, 
			event.dwThreadId, 
			DBG_CONTINUE
		);
		if (!res) {
			ERROR_LOG("Failed to continue dbg");
			break;
		}
		
	
	}

	return res;
}

bool TestRun::HandleBreakpoint(DEBUG_EVENT& Event) {
	bool res = true;
	SharedMem sharedMem;
	char* stateName = nullptr;

	do {
		auto ex = Event.u.Exception;
		res = ReadProcessMemory(
			g_TargetProcess.GetHandle(), 
			(LPCVOID)TESTS_SHARED_MEM, 
			&sharedMem,
			sizeof(sharedMem),
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to read process mem");
			break;
		}
		stateName = (char*)malloc(sharedMem.StateStrLen);
		res = res = ReadProcessMemory(
			g_TargetProcess.GetHandle(),
			(LPCVOID)(TESTS_SHARED_MEM + sizeof(SharedMem)),
			stateName,
			sharedMem.StateStrLen,
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to read process mem");
			break;
		}

		std::cout << stateName << "\n";

		switch (sharedMem.Op) {
			case TESTS_OP_CAPTURE: {
				res = URSTakeSnapshot();
				if (!res) {
					ERROR_LOG("Failed to take a snapshot");
					break;
				}
			}break;
			
			case TESTS_OP_REVERT: {
				res = URSRevertToSnapshot();
				if (!res) {
					ERROR_LOG("Failed to revert to snapshot");
					break;
				}
			}break;
		}

	} while (false);

	if (stateName != nullptr) {
		free(stateName);
	}

	return res;
}