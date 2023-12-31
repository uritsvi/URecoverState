#include "ThreadTests.h"
#include "../Error.h"
#include "../RAII/include/RAIIThread.h"
#include "../RAII/include/RAIIEvent.h"
#include "RecoverState.h"

RAIIEvent g_Event;

DWORD a(LPVOID Ctx) {
	int b = 10;

	g_Event.Wait();

	while(true){}

	return 0;
}

DWORD b(LPVOID Ctx) {
	int a = 10;
	
	g_Event.Wait();

	while(true){}

	return 0;
}

ThreadTests::ThreadTests() {

}
ThreadTests::~ThreadTests() {

}

bool ThreadTests::Run() {
	RAIIThread t1;
	RAIIThread t2;

	bool res = true;

	do {
		res = g_Event.Create();
		if (!res) {
			ERROR_LOG("Failed to create event");
			break;
		}

		DWORD tid;
		res = t1.Create(a, nullptr, tid);
		if (!res) {
			ERROR_LOG("Failed to create thread");
			break;
		}
		res = t2.Create(b, nullptr, tid);
		if (!res) {
			ERROR_LOG("Failed to create thread");
			break;
		}

		/*
		* Let the threads start
		*/
		Sleep(500);

		res = RecoverState::GetInstance().Capture();
		if (!res) {
			ERROR_LOG("Failed to capture state");
			break;
		}

		res = g_Event.Set();
		if (!res) {
			ERROR_LOG("Failed to set event");
		}
		
		/*
		* Let the threads continue execution
		*/
		Sleep(500);

		res = RecoverState::GetInstance().Recover();
		if (!res) {
			ERROR_LOG("Failed to recover state");
			break;
		}

		SUCCESS_LOG("Passed threads tests");
	
	} while (false);
	
	return res;
}