#include "ThreadTests.h"
#include "../Common/include/Error.h"

#include "RecoverState.h"
#include "DriverTest.h"
#include <winternl.h>

#pragma comment(lib, "RAII.lib")

//#define _TEST_DRIVER

ThreadTests g_ThreadTest;


int main() {
	auto module = LoadLibraryA("C:\\WINDOWS\\System32\\USER32.dll");

	std::cout << "gSharedInfo: " << GetProcAddress(module, "gSharedInfo") << "\n";




	bool res = true;
	


	do {
		res = RecoverState::GetInstance().Init();
		if (!res) {
			ERROR_LOG(("Failed to init recover state"));
		}

#if defined _TEST_DRIVER
		DriverTest driverTest;
		driverTest.Run();
#endif

		g_ThreadTest.Run();


	} while (false);

	while(1){}

	return 0;
	
}