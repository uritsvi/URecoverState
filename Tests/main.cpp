#include "ThreadTests.h"
#include "..\Error.h"

ThreadTests threadTest;

int main() {
	do {
		bool res = threadTest.Run();

		if (res) {
			SUCCESS_LOG("Passed all tests successfuly!");
			break;
		}

		ERROR_LOG("Not passed all tests!");
	} while (false);

	return 0;
}