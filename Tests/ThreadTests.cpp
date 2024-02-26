#include "ThreadTests.h"
#include "../Common/include/Error.h"

#include "../RAII/include/RAIIThread.h"
#include "../RAII/include/RAIIEvent.h"

#include "RecoverState.h"
#include <winternl.h>	
#include <ctime>

#define DEFAULT_TIME_OUT_IN_MS 7000
#define MINIMUM_TIMEOUT_IN_SC 1

#define DEFAULT_SLEEP_IN_MS 4000


ThreadTests::ThreadTests() {
}
ThreadTests::~ThreadTests() {
}


DWORD WaitForEventThreadFunc(LPVOID Ctx) {
	auto event = reinterpret_cast<RAIIEvent*>(Ctx);
	
	printf("test\n");

	event->Wait();

	printf("hello\n");


	return 0;
}

DWORD SleepThread(LPVOID Ctx) {
	


	Sleep(DEFAULT_TIME_OUT_IN_MS);



	printf("finih sleep\n");


	return 0;
}

DWORD ExitThread(LPVOID Ctx) {
	return 0;
}

void WaitTest() {

	RAIIEvent event;
	event.Create();

	RAIIThread thread;
	DWORD tid;
	thread.Create(
		WaitForEventThreadFunc, 
		&event, 
		tid
	);
	Sleep(MINIMUM_TIMEOUT_IN_SC);

	CAPTURE("cap");

	event.Set();

	printf("some msg\n");

	REVERT("rev");

	printf("finish test1\n");
}
void SleepTest() {

	bool res = true;
	do {
		RAIIThread thread;
		DWORD tid;
		thread.Create(
			SleepThread,
			nullptr,
			tid
		);

		Sleep(1000);

		CAPTURE("cap");

		RAIIThread newThread;
		newThread.Open(tid);

		DWORD exitCode;
		auto seconds = DEFAULT_TIME_OUT_IN_MS;

		res = newThread.WaitForTerminate(
			exitCode,
			seconds / 1000
		);
		if (!res) {
			printf("Failed to wait for threads termination\n");
			break;
		}

		REVERT("rev");


		printf("finish sleep test\n");
	} while (false);

	if (!res) {
		printf("error in sleep test\n");
		exit(-1);
	}
}
void InitThreadTest() {
	bool res = true;
	do {
		RAIIThread thread;
		DWORD tid;
		thread.Create(
			SleepThread,
			nullptr,
			tid
		);

		CAPTURE("cap");

		DWORD code;
		thread.WaitForTerminate(code);

		REVERT("rev");


		printf("finish init thread test\n");
	} while (false);

	if (!res) {
		printf("error in Test 2\n");
		exit(-1);
	}
}

void ExitThreadTest() {

}

void ThreadTests::Run() {

	SleepTest();
	//WaitTest();
	//InitThreadTest();
	//ExitThreadTest();
	// Suspend test 
	// apc's test
	// thread msg test 
	// thread priority test 
	// critical section test
	// try catch test and other exceptions
	// send and get message
	// read write files op's 


}
