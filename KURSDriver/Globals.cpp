#include "Globals.h"
//#include "..\Common.h"
#include "kmemory.h"

Globals::Globals() {
	m_KernelFunctions = new (POOL_FLAG_PAGED)KernelFunctions();
}
Globals::~Globals() {
	if (m_KernelFunctions != nullptr) {
		delete m_KernelFunctions;
	}
}

/*
* The most unsecure code i have evert writen
*/
NTSTATUS Globals::SetKernelFunctions(_In_ KernelFunctions* Functions) {
	NTSTATUS status = STATUS_SUCCESS;

	do {

		if (m_KernelFunctions == nullptr) {
			KdPrint(("Kernel functions are nullptr"));
			status = STATUS_UNSUCCESSFUL;
		}

		KdPrint(("sdfdsfdsf"));

		memcpy(
			m_KernelFunctions, 
			Functions, 
			sizeof(KernelFunctions)
		);

		for (UINT64 i = 0; i < m_KernelFunctions->NumOfKernelFunction; i++) {
			auto current = (UINT64)&m_KernelFunctions + i;
			if (current == 0) {
				KdPrint(("Kernel function is null"));
				status = STATUS_UNSUCCESSFUL;
				
				break;
			}


		}
		if (!NT_SUCCESS(status)) {
			break;
		}

		/*
		* Compare this function addresses
		*/

		//UNICODE_STRING aaa = RTL_CONSTANT_STRING(L"DbgPrint");

		//KdPrint(("%p", MmGetSystemRoutineAddress(&aaa)));
		//KdPrint(("%p", m_KernelFunctions->DbgPrintPtr));

	} while (false);

	return status;
}

NTSTATUS Globals::GetKernelFunctions(_Out_ KernelFunctions& Exports) {
	NTSTATUS status = STATUS_SUCCESS;

	do {
		if (m_KernelFunctions == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		KdPrint(("Errordfsdf"));

		memcpy(
			&Exports, 
			m_KernelFunctions, 
			sizeof(KernelFunctions)
);
	} while (false);

	return status;
}