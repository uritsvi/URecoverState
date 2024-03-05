#include "RAIIHandleTable.h"
#include "Globals.h"
#include "RAIIThread.h"


extern Globals* g_Globals;

RAIIHandleTable::RAIIHandleTable(_In_ PVOID64 Addr) {
	m_Addr = Addr;
	auto table = (PHANDLE_TABLE)m_Addr;


	KeEnterCriticalRegion();
	ExAcquirePushLockExclusive(&table->HandleTableLock);
}
RAIIHandleTable::~RAIIHandleTable() {
	auto table = (PHANDLE_TABLE)m_Addr;

	ExReleasePushLockExclusive(&table->HandleTableLock);
	KeLeaveCriticalRegion();
}

NTSTATUS RAIIHandleTable::DeleteEntry(_In_ HANDLE Handle) {
	NTSTATUS status = STATUS_SUCCESS;

	do {
		UNREFERENCED_PARAMETER(Handle);
	} while (false);

	return status;
}
NTSTATUS RAIIHandleTable::MoveAndDeleteEntry(
	_In_ HANDLE Src,
	_In_ HANDLE Target) {

	NTSTATUS status = STATUS_SUCCESS;
	RAIIThread thread;
	KdPrint(("helloopoooooooo\n"));

	do {
		KernelFunctions exports;
		status = g_Globals->GetKernelFunctions(exports);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to get kernel functions"));
			break;
		}

		auto targetEntry = exports.ExpLookupHandleTableEntry(
			m_Addr,
			Target
		);
		if (targetEntry == nullptr) {
			status = STATUS_NOT_IMPLEMENTED;
			KdPrint(("handle table not large enogth, not implemented"));
			break;
		}
		auto currentEntry = exports.ExpLookupHandleTableEntry(
			m_Addr,
			Src
		);
		if (currentEntry == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			KdPrint(("failed to find current entry"));
			break;
		}


		/*
		* Add a dummy entry so there is no chance the
		* target entry is the last free
		*/
		status = AddNewFreeEntry();
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to add free entry"));
			break;
		}



		/*
		* if the entry is in use, this value is null.
		* allso if it is the last entry free on the table
		*/
		if (targetEntry->NextFreeHandleEntry == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			KdPrint(("target entry is in use"));
			break;
		}


		/*
		* lock the current thread entry
		*/
		bool res = exports.ExLockHandleTableEntry(
			(PHANDLE_TABLE)m_Addr,
			currentEntry
		);
		if (!res) {
			status = STATUS_NOT_LOCKED;
			KdPrint(("failed to lock handle table entry"));
			break;
		}

		/*
		* switch the current entry and the target entry
		*/

		HANDLE_TABLE_ENTRY tempentry;
		PHANDLE_TABLE_ENTRY freeEntry;
		memcpy(
			(char*)&tempentry,
			(char*)targetEntry,
			sizeof(HANDLE_TABLE_ENTRY)
		);
		memcpy(
			(char*)targetEntry,
			(char*)currentEntry,
			sizeof(HANDLE_TABLE_ENTRY)
		);
		memcpy(
			(char*)currentEntry,
			(char*)&tempentry,
			sizeof(HANDLE_TABLE_ENTRY)
		);
		freeEntry = currentEntry;

		/*
		* the lock function changes some bit in the entry.
		* this entry is copyed to the target handle table entry.
		* so we have to unlcok it to
		*/
		exports.ExUnlockHandleTableEntry(
			(PHANDLE_TABLE)m_Addr,
			targetEntry
		);


		RemoveFromFreeList(
			targetEntry,
			freeEntry
		);

	} while (false);


	return status;
}

NTSTATUS RAIIHandleTable::SetEntryObjectBits(
	_In_ HANDLE Handle,
	_In_ PVOID Object) {

	NTSTATUS status = STATUS_SUCCESS;

	do {

		UNREFERENCED_PARAMETER(Handle);
		UNREFERENCED_PARAMETER(Object);

	} while (false);

	return status;
}

NTSTATUS RAIIHandleTable::RemoveFromFreeList(
	_In_ PVOID64 EntryAddr,
	_In_ PVOID64 NewEntryAddr) {

	NTSTATUS status = STATUS_SUCCESS;

	do {
		KernelFunctions exports;
		status = g_Globals->GetKernelFunctions(exports);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to get kernel functions"));
			break;
		}

		bool found = false;
		ULONG64 currenthandle = 0;
		while (true) {
			auto entry = exports.ExpLookupHandleTableEntry(
				m_Addr,
				(HANDLE)currenthandle++
			);
			if (entry == nullptr) {
				break;
			}

			if (entry->NextFreeHandleEntry == EntryAddr) {
				found = true;

				entry->NextFreeHandleEntry =
					(PHANDLE_TABLE_ENTRY)NewEntryAddr;

				break;
			}
		}

		if (!found) {
			status = STATUS_NOT_FOUND;
			KdPrint(("not found next free entry pointer"));
			break;
		};

	} while (false);

	return status;
}

NTSTATUS RAIIHandleTable::AddNewFreeEntry() {
	NTSTATUS status = STATUS_SUCCESS;

	do {
		KernelFunctions exports;
		status = g_Globals->GetKernelFunctions(exports);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to get kernel functions"));
			break;
		}

		HANDLE_TABLE_ENTRY empty = { 0 };
		auto handle = exports.ExCreateHandle(
			(PHANDLE_TABLE)m_Addr,
			&empty
		);
		if (handle == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			KdPrint(("Failed to create dummy entry"));
			break;
		}

		/*
		* This function returns fals for some resone,
		* but still working fine for my need
		*/
		exports.ExDestroyHandle(
			(PHANDLE_TABLE)m_Addr,
			handle,
			exports.ExpLookupHandleTableEntry(m_Addr, handle)
		);
	} while (false);

	return status;
}