#pragma once

#include <ntddk.h>

#define DRIVER_TAG 'KURS'

void* operator new(size_t Size, UINT32 PoolType);

void operator delete(void* Ptr, size_t Size);