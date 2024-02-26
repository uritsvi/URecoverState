#include "DriverTest.h"
#include "..\RAII\include\RAIIProcess.h"
#include "..\URSDriver\include\URSDriverInterface.h"
#include "../Common/include/Error.h"

#include "../RAII/include/RAIIProcess.h"
#include "../RAII/include/RAIIThread.h"
#include "../RAII/include/RAIIEvent.h"
#include "../RAII/include/RAIIHandle.h"
#include "../NTDLL/include/NTDLL.h"

#pragma comment(lib, "NTDLL.lib")
#pragma comment(lib, "RAII.lib")
#pragma comment(lib, "URSDriver.lib")

typedef BOOL (*pCompareObjectHandles)(
	_In_ HANDLE hFirstObjectHandle,
	_In_ HANDLE hSecondObjectHandle
);

pCompareObjectHandles _CompareObjectHandles;

DWORD ThreadRoutineExit(LPVOID Ctx) {
	ExitThread(0);
	return 0;
}
DWORD ThreadRoutineWhileTrue(LPVOID Ctx) {
	while (true){}
	return 0;
}
/*
* Ctx: ptr to RaiiEvent
*/
DWORD ThreadWaitOnEvent(_In_ LPVOID Ctx) {
	RAIIEvent* event = (RAIIEvent*)Ctx;
	event->Wait();

	std::cout << "Wait finished\n";

	return 0;
}

bool WaitTest() {
	
	

	bool res = true;

	do {
		RAIIThread t1;

		DWORD t1Tid = 0;
		DWORD t2Tid = 0;
		res = t1.Create(
			ThreadRoutineWhileTrue,
			nullptr, 
			t1Tid
		);
		if (!res) {
			ERROR_LOG("Failed to create t1");
			break;
		}
		/*
		* Close the t2 thread handle when done using it
		*/
		{
			RAIIThread t2;
			res = t2.Create(
				ThreadRoutineExit,
				nullptr,
				t2Tid
			);
			DWORD code;
			t2.WaitForTerminate(code);
			if (!res) {
				ERROR_LOG("Failed to create t2");
				break;
			}
		}

		res = URSDriverMoveThreadId(
			t2Tid,
			t1Tid
		);
		if (!res) {
			ERROR_LOG("Failed to move thread id");
			break;
		}

		if (t1.GetTid() != t2Tid) {
			res = false;
			ERROR_LOG("Tids are not equals");
		}
		else {
			SUCCESS_LOG("Tids are equals");
		}

		RAIIThread newT1;
		res = newT1.Open(t2Tid);
		if (!res) {
			ERROR_LOG("Failed to open thread");
			break;
		}
		if (!_CompareObjectHandles(
			t1.GetRAIIHandle()->GetHandle(),
			newT1.GetRAIIHandle()->GetHandle())) {
			
			res = false;
			ERROR_LOG("Not the same thread object");
		}
		else {
			SUCCESS_LOG("Same threads objects");
		}
		

	} while (false);

	return res;
}


DriverTest::DriverTest() {
}
DriverTest::~DriverTest() {
}

void DriverTest::Run() {
	bool res = true;

	
	
	do {
		_CompareObjectHandles =
			(pCompareObjectHandles)GetProcAddress(
				GetModuleHandleA("Kernelbase.dll"),
				"CompareObjectHandles"
			);
		if (_CompareObjectHandles == nullptr) {
			ERROR_LOG("Failed to find CompareObjectHandles function");
			break;
		}

		res = URSDriverInit();
		if (!res) {
			ERROR_LOG("Failed to load driver");
			break;
		}
		res = WaitTest();
		if (!res) {
			ERROR_LOG("Test 1 failed");
		}
	



	} while (false);
	
}
