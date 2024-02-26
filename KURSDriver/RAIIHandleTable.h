#pragma once

#include <ntddk.h>

#include "Globals.h"

class RAIIHandleTable
{

public:

	/*
	* Locks the table on create, and free on destruct
	*/
	RAIIHandleTable(_In_ PVOID64 Addr);
	~RAIIHandleTable();

	NTSTATUS MoveAndDeleteEntry(
		_In_ HANDLE Src, 
		_In_ HANDLE Target
	);

	NTSTATUS SetEntryObjectBits(
		_In_ HANDLE Handle, 
		_In_ PVOID Object
	);

private:
	NTSTATUS DeleteEntry(_In_ HANDLE Handle);

	NTSTATUS RemoveFromFreeList(
		_In_ PVOID64 EntryAddr, 
		_In_ PVOID64 NewEntryAddr
	);

	NTSTATUS AddNewFreeEntry();


private:
	EX_PUSH_LOCK m_TableLock;
	PVOID64 m_Addr;
};

