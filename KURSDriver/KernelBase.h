#pragma once

#include <ntddk.h>

NTSTATUS GetKernelBase(
	_Out_ PVOID64& Addr,
	_Out_ ULONG& Size
);