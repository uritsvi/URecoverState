#pragma once

#include <ntddk.h>

#include "RAIIBuffer.h"

/*
*	The CloseHandlesForAllProcesses function suspends every 
*	process who has an open handle to the object until the 
*	RecreateHandlesForAllProcesses function is called
*/

class KernelHandles
{
	/*
public:
	NTSTATUS CloseHandlesForAllProcesses(PVOID64 Obj);

	NTSTATUS RecreateHandlesForAllProcesses(PVOID NewObj);

private:
	NTSTATUS GetSystemHandleInfo(_Out_ RAIIBuffer& Buffer);
	*/
};

