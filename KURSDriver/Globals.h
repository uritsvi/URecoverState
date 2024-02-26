#pragma once

#include <ntddk.h>
#include "..\Common\include\Common.h"

class Globals
{

public:
	Globals();
	~Globals();

	NTSTATUS SetKernelFunctions(_In_ KernelFunctions* Functions);
	NTSTATUS GetKernelFunctions(_Out_ KernelFunctions& KernelFunctions);

private:
	KernelFunctions* m_KernelFunctions;
};

