#pragma once

#include <memory>

#include "../Common/include/SharedMemory.h"

class DllFunctions;

typedef bool(*DllFunction)(_In_ DllFunctions* Instance, _In_ LPARAM LParam);

class DllFunctions
{
public:
	bool Init(_In_ std::shared_ptr<SharedMemory> SharedMemory);

	bool UMapViewOfFile(_In_ LPARAM LParam);
	bool UUnMapViewOfFile(_In_ LPARAM LParam);


private:
	std::shared_ptr<SharedMemory> m_SharedMemory;
};


/*
* Function code: MAP_FILE_FUNCTION
*/
bool UMapViewOfFile(
	_In_ DllFunctions* Instance,
	_In_ LPARAM LParam
);

/*
* Function code: UNMAP_FILE_FUNCTION
*/
bool UUnMapViewOfFile(
	_In_ DllFunctions* Instance,
	_In_ LPARAM LParam
);



