#include "ThreadTests.h"
#include "..\Error.h"
#include "RecoverState.h"

#pragma comment(lib, "RAII.lib")

ThreadTests threadTest;

int main() {
	bool res = true;

	do {
		res = RecoverState::GetInstance().Init();
		if (!res) {
			ERROR_LOG(("Failed to init recover state"));
		}

		res = threadTest.Run();

		if (res) {
			SUCCESS_LOG("Passed all tests successfuly!");
			break;
		}

		ERROR_LOG("Not passed all tests!");
	} while (false);

	return 0;
}