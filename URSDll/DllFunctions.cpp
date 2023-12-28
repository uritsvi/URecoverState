#include "DllFunctions.h"
#include "../Error.h"

bool UMapViewOfFile(
	_In_ DllFunctions* Instance,
	_In_ LPARAM LParam) {

	return Instance->UMapViewOfFile(LParam);
}

bool UUnMapViewOfFile(
	_In_ DllFunctions* Instance,
	_In_ LPARAM LParam) {

	return Instance->UUnMapViewOfFile(LParam);

}

bool DllFunctions::Init(_In_ std::shared_ptr<SharedMemory> SharedMemory) {
	m_SharedMemory = SharedMemory;
	
	return true;
}

bool DllFunctions::UMapViewOfFile(_In_ LPARAM LParam) {
	bool res = true;

	char buffer[MAX_PATH];
	ULONG64 strLen;

	do {

		res = m_SharedMemory->ReadStr(
			MAX_PATH,
			0,
			buffer,
			strLen
		);



		if (!res) {
			ERROR_LOG("Failed to read str");
			break;
		}



		std::cout << LParam << " " << buffer << "\n";

	} while (false);

	return res;
}
bool DllFunctions::UUnMapViewOfFile(_In_ LPARAM LParam) {
	bool res = true;

	do {
		std::cout << LParam << "\n";

	} while (false);

	return res;
}