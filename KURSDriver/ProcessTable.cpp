#include "ProcessTable.h"
#include "..\Common\include\Common.h"
#include "Globals.h"
#include "RAIIHandleTable.h"
#include "RAIIProcess.h"

extern Globals* g_Globals;

ProcessTable::ProcessTable() {

}
ProcessTable::~ProcessTable() {

}

NTSTATUS ProcessTable::ChnageProcessCid(
	_In_ RAIIProcess& Process,
	_In_ HANDLE Pid) {

	UNREFERENCED_PARAMETER(Pid);
	UNREFERENCED_PARAMETER(Process);

	NTSTATUS status = STATUS_SUCCESS;

	do{



	} while (false);

	return status;
}

NTSTATUS ProcessTable::ChangeProcessId(ChangeProcessIdParams* Info) {
	NTSTATUS status = STATUS_SUCCESS;

	do {
		KernelFunctions exports;
		status = g_Globals->GetKernelFunctions(exports);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to get kernel functions"));
			break;
		}

		RAIIHandleTable pspCidTable(*(PVOID64*)exports.pPspCidTable);

		if (Info->ProcessId == PsGetCurrentProcessId() || Info->TargetId == PsGetCurrentProcessId()) {
			KdPrint(("Cant change current thread id, Or to current thread id"));
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		RAIIProcess srcProcess;
		status = srcProcess.Open(Info->ProcessId);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to open src thread"));
			break;
		}

		status = pspCidTable.MoveAndDeleteEntry(
			Info->ProcessId,
			Info->TargetId
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to move thread id"));
			break;
		}


		RAIIProcess process;
		status = process.Open(Info->TargetId);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to open target thread"));
			break;
		}

		status = ChnageProcessCid(
			process,
			Info->TargetId
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to change thread cid"));
			break;
		}
	} while (false);

	return status;
}