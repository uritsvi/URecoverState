#include "libraries\include\MinHook.h"

#pragma comment(lib, "libMinHook-x64-v90-md.lib")

#include "Hook.h"
#include "..\Common\include\Error.h"

typedef NTSTATUS(*pNtClose)(HANDLE Handle);

pNtClose g_OriginalNtClose;

NTSTATUS MyNtClose(HANDLE Handle) {
	return g_OriginalNtClose(Handle);
}

Hook::Hook() {
	MH_Initialize();
}
Hook::~Hook() {
	MH_Uninitialize();
}

bool Hook::PerformHooks() {
	bool res = true;

	do {
		auto status = MH_CreateHookApi(
			L"ntdll.dll", 
			"NtClose", 
			MyNtClose, 
			(LPVOID*)&g_OriginalNtClose
		);
		if (status != MH_OK) {
			ERROR_LOG("Failed to hook NtCloseHandle");
			res = false;
			break;
		}

	} while (false);

	return res;
}
bool Hook::DisablesHooks() {
	bool res = true;

	do {
		auto status = MH_DisableHook(MyNtClose);
		ERROR_LOG("Failed to disable hook NtCloseHandle");
		if (status != MH_OK) {
			res = false;
			break;
		}
	} while (false);

	return res;
}