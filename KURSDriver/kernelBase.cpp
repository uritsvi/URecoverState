#include "KernelBase.h"
#include "kmemory.h"

#define SYSTEM_MODULE_INFO 0x0b
#define NTOSKRNL_PATH "\\SystemRoot\\system32\\ntoskrnl.exe"

extern "C" NTSTATUS NTAPI ZwQuerySystemInformation(
	ULONG SystemInformationClass, 
	PVOID SystemInformation, 
	ULONG SystemInformationLength, 
	PULONG ReturnLength
);

typedef unsigned short      WORD;
typedef unsigned char      BYTE;

typedef struct _SYSTEM_MODULE {
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];


} SYSTEM_MODULE, * PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG                ModulesCount;
	SYSTEM_MODULE        Modules[1];


} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

NTSTATUS GetKernelBase(
	_Out_ PVOID64& Addr, 
	_Out_ ULONG& Size)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG size = 0;
	PVOID buffer = nullptr;
	PSYSTEM_MODULE_INFORMATION modulesInfo = nullptr;
	do {
		
		status = ZwQuerySystemInformation(
			SYSTEM_MODULE_INFO, 
			&size, 
			0, 
			&size
		);
		buffer = ExAllocatePool2(
			POOL_FLAG_NON_PAGED,
			size,
			DRIVER_TAG
		);
		if (buffer == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			KdPrint(("Failed to allocate memory"));
			break;
		}

		status = ZwQuerySystemInformation(
			SYSTEM_MODULE_INFO,
			buffer,
			size,
			&size
		);
		if (!NT_SUCCESS(status)) {
			KdPrint(("Failed to query modules info"));
			break;
		}

		modulesInfo = (PSYSTEM_MODULE_INFORMATION)buffer;
		
		status = STATUS_NOT_FOUND;
		for (ULONG i = 0; i < modulesInfo->ModulesCount; i++) {
			
			auto current = modulesInfo->Modules[i];
			if (strcmp((const char*)current.FullPathName, NTOSKRNL_PATH) != 0) {
				continue;
			}

			Addr = current.ImageBase;
			Size = (DWORD64)current.ImageSize;

			KdPrint(("%ul", current.ImageSize));

			status = STATUS_SUCCESS;

			break;
		}

	} while (false);

	if (buffer != nullptr) {
		ExFreePool(buffer);
	}

	return status;
}