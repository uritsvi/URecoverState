#include "kmemory.h"

void* operator new(size_t Size, UINT32 PoolType) {
	return ExAllocatePool2(
		PoolType,
		Size,
		DRIVER_TAG
	);
}

void operator delete(void* Ptr, size_t Size) {
	UNREFERENCED_PARAMETER(Size);

	ExFreePool(Ptr);
}