#include <ntifs.h>

#include "ThreadsTable.h"
#include "RAIIHandleTable.h"
#include "RAIIThread.h"
#include "Globals.h"
#include "KernelHandles.h"

EXTERN_C NTKERNELAPI PVOID NTAPI PsGetThreadTeb(PETHREAD Thread);

extern Globals* g_Globals;

ThreadsTable::ThreadsTable() {
}
ThreadsTable::~ThreadsTable() {
}


NTSTATUS ThreadsTable::ChanageThreadCid(
	_In_ RAIIThread& Thread,
	_In_ HANDLE NewId) {


	NTSTATUS status = STATUS_SUCCESS;

	do {
		/*
		* Cid ptr offset
		*/
		auto pcid = (PCLIENT_ID)((char*)Thread.GetEthread() + 0x478);
		if (pcid == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			KdPrint(("failed to find target ethread cid"));
			break;
		}
		pcid->UniqueThread = NewId;

		/*
		* Cid ptr offset
		*/
		/*
		auto teb = PsGetThreadTeb((PETHREAD)Thread.GetEthread());

		pcid = (PCLIENT_ID)((char*)teb + 0x40);

		pcid->UniqueThread = NewId;
		*/
	} while (false);

	return status;
}

NTSTATUS ThreadsTable::ChangeThreadTEB(_In_ ChangeThreadTEBParams* NewTeb) {
	NTSTATUS status = STATUS_SUCCESS;

	do {
		PKTHREAD kthread;
		status = PsLookupThreadByThreadId(
			NewTeb->ThreadId, 
			&kthread
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to lookup thread by id"));
			break;
		}

#define TEB_OFFSET 0x0f0

		memcpy(
			(char*)kthread + TEB_OFFSET, 
			&NewTeb->NewTEB, 
			sizeof(NewTeb->NewTEB)
		);
		
	} while (false);

	return status;
}

NTSTATUS ThreadsTable::MoveThreadId(_In_ MoveThreadIdParams* Info) {

	NTSTATUS status = STATUS_SUCCESS;

	do {
		KernelFunctions exports;
		status = g_Globals->GetKernelFunctions(exports);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to get kernel functions"));
			break;
		}

		RAIIHandleTable pspCidTable(*(PVOID64*)exports.pPspCidTable);

		auto currentId = PsGetCurrentThreadId();
		if (Info->SrcId == currentId || Info->TargetId == currentId) {
			KdPrint(("Cant change current thread id, Or to current thread id"));
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		RAIIThread srcThread;
		status = srcThread.Open(Info->SrcId);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to open src thread"));
			break;
		}
		/*
		* Close all handles pointing to the target ethread
		*/
		/*
		KernelHandles kernelHandles;
		status = kernelHandles.CloseHandlesForAllProcesses(
			srcThread.GetEthread()
		);
		if (NT_SUCCESS(status)) {
			KdPrint(("Failed to close all handles to thread"));
			break;
		}
		*/

		status = pspCidTable.MoveAndDeleteEntry(
			Info->SrcId, 
			Info->TargetId
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to move thread id"));
			break;
		}


		RAIIThread thread;
		status = thread.Open(Info->TargetId);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to open target thread"));
			break;
		}

		status = ChanageThreadCid(
			thread, 
			Info->TargetId
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to change thread cid"));
			break;
		}

		/*
		status = kernelHandles.RecreateHandlesForAllProcesses(thread.GetEthread());
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to recreate handles to threads"));
			break;
		}
		*/


		//TODO: Check witch processes have a handle to the threadand change there bits

		KdPrint(("Move thread id success"));
	} while (false);

	return status;
}
