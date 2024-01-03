#include "URSDLL.h"
#include "Error.h"
#include "TargetProcess.h"
#include "Common.h"

#include "RAII/include/RAIIThread.h"

#define URS_DLL_NAME "URSDll.dll"



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

		res = process->LoadDllToTargetProcess(
			path, 
			m_RemoteThreadTid
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
		LPARAM lparam = 0;
		res = m_SharedMemory.ReciveMsg(
			msg,
			wparam,
			lparam
		);
		if (!res) {
			ERROR_LOG("Failed to recive msg");
			break;
		}
		if(msg != FINISH_OP || wparam != SUCCESS_OP){
			ERROR_LOG("Failed to init remote dll");
			break;
		}

		m_RemoteThreadTid = lparam;
		m_SharedMemory.SetTargetTid(lparam);

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

		res = m_SharedMemory.SendMsg(
			FUNCTION_CALL_OP, 
			Function, 
			LParam
		);
		m_SharedMemory.ReciveMsg(ret, wparam, lparam);
	} while (false);

	return res;
}

bool URSDLL::MapFileToTarget(
	_In_ std::string Path,
	_In_ UINT64 Addr) {

	bool res = true;

	do {
		res = m_SharedMemory.WriteStr(Path.c_str(), 0);
		if (!res) {
			ERROR_LOG("Failed to write path");
			break;
		}

		UINT ret = SUCCESS_OP;
		CallDllFunction(
			(LPARAM)Addr, 
			MAP_FILE_FUNCTION, 
			ret
		);
		if (!res) {
			ERROR_LOG("Failed to call dll function");
			break;
		}
		if (ret != SUCCESS_OP) {
			ERROR_LOG("Failed to map file to target, error: " + ret);
			break;
		}

	} while (false);

	return res;
}
bool URSDLL::UnmapFileFromTarget(_In_ UINT64 Addr) {
	bool res = true;

	do {
		UINT ret = SUCCESS_OP;
		res = CallDllFunction(
			(LPARAM)Addr, 
			UNMAP_FILE_FUNCTION, 
			ret
		);
		if (!res) {
			ERROR_LOG("Failed to call to a dll function");
			break;
		}
		if (ret != SUCCESS_OP) {
			ERROR_LOG("Failed to unmap file from target, error: " + ret);
			break;
		}

	} while (false);

	return res;
}

bool URSDLL::ResumeDllThread() {
	bool res = true;
	
	do {
		RAIIThread thread;
		res =thread.Open(m_RemoteThreadTid);
		if (!res) {
			ERROR_LOG("Failed to open dll thread");
			break;
		}

		res = thread.Resume();
		if (!res) {
			ERROR_LOG("Failed to resume thread");
			break;
		}

	} while (false);

	return res;
}