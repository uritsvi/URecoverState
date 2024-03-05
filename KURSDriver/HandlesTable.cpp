#include <ntifs.h>

#include "HandlesTable.h"
#include "RAIIHandleTable.h"

HandlesTable::HandlesTable() {
}
HandlesTable::~HandlesTable() {
}

NTSTATUS HandlesTable::ChnageHandle(_In_ ChangeHandleParams* Params) {
	UNREFERENCED_PARAMETER(Params);

	NTSTATUS status = STATUS_SUCCESS;

#define OBJECT_TABLE_OFFSET 0x570

	KdPrint(("Wtffffffffff\n"));
	do {
		PEPROCESS process;
		status = PsLookupProcessByProcessId(
			Params->Pid, 
			&process
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to lookup target process"));
			break;
		}

		PVOID64 objectTableAddr;
		memcpy(
			&objectTableAddr,
			(char*)process + OBJECT_TABLE_OFFSET,
			sizeof(PVOID64)
		);
		RAIIHandleTable handleTable(objectTableAddr);
		status = handleTable.MoveAndDeleteEntry(
			Params->SrcHandle, 
			Params->TargetHandle
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to move handle"));
			break;
		}

	} while (false);

	return status;
}