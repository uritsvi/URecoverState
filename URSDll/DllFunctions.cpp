#include "DllFunctions.h"
#include "..\Common\include\Error.h"
#include "..\Common\include\Common.h"
#include "..\NTDLL\include\NTDLL.h"
#include "..\RAII\include\RAIIBuffer.h"
#include "..\RAII\include\RAIIThread.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "intrin.h"



bool UCloneTargetProcess(
	_In_ DllFunctions* Instance,
	_In_ LPARAM LParam) {

	return Instance->CloneTargetProcess();
}

bool DllFunctions::Init(_In_ std::shared_ptr<SharedMemory> SharedMemory) {
	m_SharedMemory = SharedMemory;

	return true;
}

bool DllFunctions::CloneTargetProcess() {

	
	bool res = true;

	do {

		RTL_USER_PROCESS_INFORMATION info;
		NTSTATUS status = NTDLL::GetInstance().RtlCloneUserProcess(
			RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES, 
			nullptr, 
			nullptr, 
			nullptr, 
			&info
		);
		if (status == RTL_CLONE_CHILD) {
			res = FreeConsole();
			AllocConsole();

			FILE* f;
			freopen_s(&f, "CON", "w", stdout);

			printf("new console\n");
			
			while(true){}

			return 0;
		}

		DWORD pid = (DWORD)info.ClientId.UniqueProcess;
		res = m_SharedMemory->Write(
			0, 
			sizeof(DWORD), 
			(char*)&pid
		);

		if (!res) {
			ERROR_LOG("Failed to write to shared mem");
			break;
		}
	} while (false);


	return res;
}

