#include "KernelHandles.h"
#include "RAIIBuffer.h"
#include "Def.h"

/*
#define NUM_OF_REPEATE 3


NTSTATUS KernelHandles::GetSystemHandleInfo(_Out_ RAIIBuffer& Buffer) {

	NTSTATUS status = STATUS_SUCCESS;

	do {
		SYSTEM_HANDLE_INFORMATION dummy;

		ULONG size = 0;
		ZwQuerySystemInformation(
			SystemHandleInformation,
			(PVOID64)&dummy,
			sizeof(dummy),
			&size
		);

		RAIIBuffer buffer(size);
		auto handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer.GetAddr();
		if (handleInfo == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			KdPrint(("Failed to get raii buffer addr"));
			break;
		}

		ULONG newSize;
		ZwQuerySystemInformation(
			SystemHandleInformation,
			handleInfo,
			size,
			&newSize
		);
		if (newSize != size) {
			KdPrint(("Buffers size miss matched"));
			break;
		}

		buffer.Move(Buffer);

	} while (false);

	return status;
}

NTSTATUS KernelHandles::CloseHandlesForAllProcesses(PVOID64 Obj) {
	UNREFERENCED_PARAMETER(Obj);

	NTSTATUS status = STATUS_SUCCESS;

	do {

		RAIIBuffer buffer;
		for (int i = 0; i < NUM_OF_REPEATE; i++) {
			status = GetSystemHandleInfo(buffer);
			if (!NT_SUCCESS(status)) {
				KdPrint(("Failed to get system handle info"));
				break;
			}
		}
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to get system handle info"));
			break;
		}
		KdPrint(("%d\n", PsGetCurrentThreadId()));
		auto handleInfo = 
			(PSYSTEM_HANDLE_INFORMATION)buffer.GetAddr();

		for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
			auto current = handleInfo->Handles[i];

			if (current.Object == Obj){
				KdPrint(("%d\n", current.UniqueProcessId));
			}
		}

	} while (false);

	return status;
}

NTSTATUS KernelHandles::RecreateHandlesForAllProcesses(PVOID NewObj) {
	UNREFERENCED_PARAMETER(NewObj);

	NTSTATUS status = STATUS_SUCCESS;

	do {
	} while (false);

	return status;
}
*/