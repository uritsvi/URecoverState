#pragma once

#include <ntddk.h>
#include "..\Common\include\Common.h"
#include "RAIIProcess.h"

class ProcessTable
{
	
public:
	ProcessTable();
	~ProcessTable();

	NTSTATUS ChangeProcessId(ChangeProcessIdParams* Params);

private:
	NTSTATUS ChnageProcessCid(
		_In_ RAIIProcess& Process,
		_In_ HANDLE Pid
	);

};

