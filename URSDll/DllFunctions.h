#pragma once

#include <memory>

#include "../Common/include/SharedMemory.h"

class DllFunctions;

typedef bool(*DllFunction)(_In_ DllFunctions* Instance, _In_ LPARAM LParam);

class DllFunctions
{
public:
	bool Init(_In_ std::shared_ptr<SharedMemory> SharedMemory);

	bool CloneTargetProcess();

private:
	std::shared_ptr<SharedMemory> m_SharedMemory;
	
};


bool UCloneTargetProcess(
	_In_ DllFunctions* Instance,
	_In_ LPARAM LParam);

