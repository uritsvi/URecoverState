#pragma once

#include <ntddk.h>

#include "..\Common\include\Common.h"
#include "RAIIThread.h"

#include "Globals.h"


class ThreadsTable
{
public:
	ThreadsTable();
	~ThreadsTable();

	NTSTATUS MoveThreadId(_In_ MoveThreadIdParams* Info);
	NTSTATUS ChangeThreadTEB(_In_ ChangeThreadTEBParams* NewTeb);

private:
	NTSTATUS ChanageThreadCid(
		_In_ RAIIThread& Thread,
		_In_ HANDLE NewId
	);


};

