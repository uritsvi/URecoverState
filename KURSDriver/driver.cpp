#include <ntifs.h>
#include <ntddk.h>


#include "..\Common\include\Common.h"
#include "Globals.h"
#include "kmemory.h"
#include "KernelBase.h"
#include "ThreadsTable.h"
#include "ProcessTable.h"
#include "HandlesTable.h"

#define DRIVER_NAME L"\\Driver\\KURS"
#define DEVICE_NAME L"\\Device\\KURS"
#define SYM_LINK_NAME L"\\DosDevices\\KURS"

Globals* g_Globals;
ThreadsTable* g_ThreadsTable;
ProcessTable* g_ProcessTable;

bool g_ManualyMapped;

extern "C" NTKERNELAPI
NTSTATUS
IoCreateDriver(
	IN PUNICODE_STRING DriverName, OPTIONAL
	IN PDRIVER_INITIALIZE InitializationFunction
);

NTSTATUS 
DriverInit(
	_In_ struct _DRIVER_OBJECT* DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);

typedef VOID(*pKiTryUnwaitThread)(
	IN PRKTHREAD Thread,
	IN LONG_PTR WaitStatus,
	IN KPRIORITY Increment,
	IN PLIST_ENTRY ThreadList OPTIONAL
	);

NTSTATUS Control(PDEVICE_OBJECT Device, PIRP Irp);

void Thread(_In_ PVOID StartContext);

NTSTATUS CompleteIrp(
	_In_ PIRP Irp,
	_In_ NTSTATUS status,
	_In_ ULONG_PTR info = 0) {

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	return status;
}

NTSTATUS CreateClose(
	_In_ PDEVICE_OBJECT Device, 
	_In_ PIRP Irp) {
	
	UNREFERENCED_PARAMETER(Device);
	UNREFERENCED_PARAMETER(Irp);

	NTSTATUS status = STATUS_SUCCESS;

	return CompleteIrp(
		Irp,
		status, 
		0
	);
}

NTSTATUS Control(
	_In_ PDEVICE_OBJECT Device,
	_In_ PIRP Irp) {

	UNREFERENCED_PARAMETER(Device);
	UNREFERENCED_PARAMETER(Irp);

	NTSTATUS status = STATUS_SUCCESS;

	int outputBufferSize = 0;

	auto sp = IoGetCurrentIrpStackLocation(Irp);
	auto dic = sp->Parameters.DeviceIoControl;

	__try {
		do {

			switch (dic.IoControlCode) {
			case IOCTL_LOAD_SYMBOLS: {
				KdPrint(("Load kernel symbols"));

				if (dic.InputBufferLength != sizeof(KernelFunctions) ||
					dic.Type3InputBuffer == nullptr) {

					status = STATUS_INVALID_PARAMETER;
					break;
				}

				status = g_Globals->SetKernelFunctions(
					(KernelFunctions*)dic.Type3InputBuffer
				);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to set kernel function"));
					break;
				}

				KdPrint(("Load kernel symbols success"));


			}break;
			case IOCTL_GET_KERNEL_BASE: {
				if (dic.OutputBufferLength != sizeof(KernelInfo)) {
					status = STATUS_INVALID_PARAMETER;
					break;
				}


				PVOID64 addr;
				ULONG size;
				status = GetKernelBase(
					addr,
					size
				);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to get kernel base"));
					break;
				}

				KernelInfo* kernelInfo = (KernelInfo*)Irp->UserBuffer;

				kernelInfo->BaseAddr = addr;
				kernelInfo->Size = size;

				outputBufferSize = sizeof(KernelInfo);

				KdPrint(("Get kernel base success"));
			}break;
			case IOCTL_MOVE_THREAD_ID: {
				if (dic.InputBufferLength != sizeof(MoveThreadIdParams)) {
					status = STATUS_INVALID_PARAMETER;
					break;
				}
				status = 
					g_ThreadsTable->MoveThreadId((MoveThreadIdParams*)dic.Type3InputBuffer);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to move thread id"));
					break;
				}

				KdPrint(("Move thread id success"));
				
			}break;

			case IOCTL_CHANGE_HANDLE: {

				if (dic.InputBufferLength != sizeof(ChangeHandleParams)) {
					status = STATUS_INVALID_PARAMETER;
					break;
				}

				HandlesTable table;
				status = table.ChnageHandle((ChangeHandleParams*)dic.Type3InputBuffer);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to change handle"));
					break;
				}

				KdPrint(("Wtffffffffff\n"));
				KdPrint(("Change handle success "));
			}break;

			case IOCTL_CHANGE_PROCESS_ID: {
				if (dic.InputBufferLength != sizeof(ChangeProcessIdParams)) {
					status = STATUS_INVALID_PARAMETER;
					break;
				}
				
				status = g_ProcessTable->ChangeProcessId(
					(ChangeProcessIdParams*)dic.Type3InputBuffer
				);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to change process id"));
					break;
				}
				
				KdPrint(("Chnage peb success"));
			}break;


			case IOCTL_CHANGE_THREAD_TEB: {
				auto params = (ChangeThreadTEBParams*)dic.Type3InputBuffer;
				status = g_ThreadsTable->ChangeThreadTEB(params);
				if (!NT_SUCCESS(status)) {
					KdPrint(("Failed to change thread teb"));
					break;
				}

				KdPrint(("Chnage teb success"));
			}break;

			default:status = STATUS_INVALID_DEVICE_REQUEST; break;
			

			}
		} while (false);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){
		
		KdPrint(("Exception while handeling ioctl"));
		status = STATUS_UNSUCCESSFUL;
		outputBufferSize = 0;
	}
	return CompleteIrp(
		Irp,
		status,
		outputBufferSize
	);

}

void CleanUp(_In_ PDRIVER_OBJECT Driver) {
	NTSTATUS status;

	UNICODE_STRING deviceName =
		RTL_CONSTANT_STRING(DEVICE_NAME);
	UNICODE_STRING symLink =
		RTL_CONSTANT_STRING(SYM_LINK_NAME);

		status = IoDeleteSymbolicLink(&symLink);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to delete sym link"));
		}
		IoDeleteDevice(Driver->DeviceObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to unload driver"));
	}
	if (g_Globals != nullptr) {
		delete g_Globals;
	}
	if (g_ThreadsTable != nullptr) {
		delete g_ThreadsTable;
	}
	if (g_ProcessTable != nullptr) {
		delete g_ProcessTable;
	}
}

extern "C" NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT Driver, 
	_In_ PUNICODE_STRING Reg) {

	UNREFERENCED_PARAMETER(Driver);
	UNREFERENCED_PARAMETER(Reg);

	NTSTATUS status = STATUS_SUCCESS;
	g_ManualyMapped = false;


	do {
		//
		// For drivers loaded with kdmapper
		//
		if (Driver == nullptr) {
			g_ManualyMapped = true;
	
			HANDLE thread;
			status = PsCreateSystemThread(
				&thread, 
				THREAD_ALL_ACCESS, 
				nullptr, 
				nullptr, 
				nullptr, 
				Thread, 
				nullptr
			);
			if (!NT_SUCCESS(status)) {
				KdPrint(("Failed to create system thread\n"));
			}
			break;
		}
		Driver->DriverUnload = CleanUp;
		status = DriverInit(
			Driver, 
			Reg
		);
	} while (false);

	return status;
}

void Thread(_In_ PVOID StartContext) {
	UNREFERENCED_PARAMETER(StartContext);

	NTSTATUS status;
	
	do {
		UNICODE_STRING driverName;
		RtlInitUnicodeString(
			&driverName,
			DRIVER_NAME
		);

		status = IoCreateDriver(
			&driverName,
			DriverInit
		);
	} while (false);

	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create driver\n"));
	}
}

NTSTATUS DriverInit(
	_In_ struct _DRIVER_OBJECT* DriverObject,
	_In_ PUNICODE_STRING RegistryPath) {

	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	KdPrint(("URSDriver loadd\n"));

	NTSTATUS status = STATUS_SUCCESS;
	do {

		PDEVICE_OBJECT dev;

		UNICODE_STRING deviceName = 
			RTL_CONSTANT_STRING(DEVICE_NAME);
		UNICODE_STRING symLink =
				RTL_CONSTANT_STRING(SYM_LINK_NAME);
		status = IoCreateDevice(
			DriverObject, 
			0, 
			&deviceName, 
			FILE_DEVICE_UNKNOWN, 
			FILE_DEVICE_SECURE_OPEN,
			FALSE, 
			&dev
		);

		if (!NT_SUCCESS(status)) {
			break;
		}

		status = IoCreateSymbolicLink(
			&symLink, 
			&deviceName
		);
		if (!NT_SUCCESS(status)) {
			break;
		}


		DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Control;

		if (g_ManualyMapped) {
			DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;
			DriverObject->Flags |= DO_DIRECT_IO;
			KdPrint(("KURS manualy mappedaaaa"));
		}

		g_Globals = new (POOL_FLAG_PAGED)Globals();
		g_ThreadsTable = new (POOL_FLAG_PAGED)ThreadsTable();
		g_ProcessTable = new (POOL_FLAG_PAGED)ProcessTable();

		if (g_Globals == nullptr || g_ThreadsTable == nullptr) {
			KdPrint(("Failed to allocate memory"));
			status = STATUS_UNSUCCESSFUL;
			break;
		}

	} while (false);

	if (!NT_SUCCESS(status)) {
		KdPrint(("Error 0x%08X\n", status));
		CleanUp(DriverObject);
	}

	return status;
}



