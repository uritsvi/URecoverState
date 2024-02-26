#include "URSDLL.h"
#include "..\Common\include\Error.h"
#include "..\RAII\include\RAIIProcess.h"
#include "..\Common\include\TargetProcess.h"
#include "..\Common\include\Common.h"
#include "..\ntdll\include\NTDLL.h"

#include "..\RAII/include/RAIIThread.h"

#define URS_DLL_NAME "URSDll.dll"


URSDLL& URSDLL::GetInstance() {
	static URSDLL instance;
	return instance;
}

bool URSDLL::IsLoaded() {
	return m_IsLoaded;
}

bool URSDLL::Load() {
	bool res = true;

	do {
		MSG msg;
		PeekMessage(&msg, nullptr, 0, 0, FALSE);

		res = InitSharedMemory();
		if (!res) {
			ERROR_LOG("Failed to init shared memory");
			break;
		}



		auto process = TargetProcess::GetInstance().GetProcess();
		res = process->Suspend();
		if (!res) {
			ERROR_LOG("Failed to suspend process");
			break;
		}

		char path[MAX_PATH];
		res = GetFullPathNameA(
			URS_DLL_NAME,
			MAX_PATH,
			path,
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to get dll path");
			break;
		}

		DWORD tid;
		res = process->LoadDllToTargetProcess(
			path,
			tid
		);
		if (!res) {
			ERROR_LOG("Failed to load urs dll");
			break;
		}


		res = FinishDllInit();
		if (!res) {
			ERROR_LOG("Failed to load remote dll");
			break;
		}

		res = process->Resume();
		if (!res) {
			ERROR_LOG("Failed to resume target process");
			break;
		}

	} while (false);

	m_IsLoaded = res;

	return res;

}

bool URSDLL::InitSharedMemory() {

	bool res = true;

	do {
		res = m_SharedMemory.Create();
		if (!res) {
			ERROR_LOG("Failed to create shared memory chanel");
			break;
		}

		/*
		* Should not be here
		*/
		auto tid = GetCurrentThreadId();
		res = m_SharedMemory.Write(
			0,
			sizeof(tid),
			(char*)&tid
		);
		if (!res) {
			ERROR_LOG("Faled to write tid to shared memory");
			break;
		}

	} while (false);

	return res;
}


bool URSDLL::FinishDllInit() {
	bool res = true;

	do {

		UINT msg = 0;
		WPARAM wparam = 0;
		LPARAM tid = 0;
		res = m_SharedMemory.ReciveMsg(
			msg,
			wparam,
			tid
		);
		if (!res) {
			ERROR_LOG("Failed to recive msg");
			break;
		}
		if (msg != FINISH_OP || wparam != SUCCESS_OP) {
			ERROR_LOG("Failed to init remote dll");
			break;
		}
		
		m_Tid = (DWORD)tid;

		std::cout << "Dll " << std::to_string(tid) << "\n";


		m_SharedMemory.SetTargetTid(m_Tid);

	} while (false);

	return res;
}

bool URSDLL::CallDllFunction(
	_In_ LPARAM LParam,
	_In_ UINT Function,
	_Out_ UINT& ReturnValue) {

	bool res = true;

	do {
		UINT ret;
		WPARAM wparam;
		LPARAM lparam;

		RAIIThread m_RemoteThread;
		res = m_RemoteThread.Open(m_Tid);
		if (!res) {
			ERROR_LOG("Failed to open thread");
			break;
		}

		res = m_RemoteThread.FourceResume();
		if (!res) {
			ERROR_LOG("Failed to fource resume dll thread");
			break;
		}

		res = m_SharedMemory.SendMsg(
			FUNCTION_CALL_OP,
			Function,
			LParam
		);
		m_SharedMemory.ReciveMsg(ret, wparam, lparam);
	} while (false);

	return res;
}

/*
void URSDLL::GetDllThreadId(DWORD& Tid) {
	Tid = m_Tid;
}
*/


bool URSDLL::CloneTargetProcess(_Out_ DWORD& Pid) {
	bool res = true;

	do {
		UINT ret = 0;
		res = CallDllFunction(
			GetCurrentThreadId(),
			UCLONE_TARGET_PROCESS, 
			ret
		);
		if (!res) {
			ERROR_LOG("Failed to clone process");
			break;
		}

		CLIENT_ID clientId;
		res = m_SharedMemory.Read(
			0, 
			sizeof(clientId),
			(char*)&clientId
		);
		Pid = (DWORD)clientId.UniqueProcess;
		if (!res) {
			ERROR_LOG("Failed to read from shared mem");
			break;
		}
	} while (false);

	return res;
}