#include "URS.h"
#include "..\Common\include\Error.h"


#include "..\RAII\include\RAIIProcess.h"
#include "..\Common\include\TargetProcess.h"

#include "..\RAII\include\RAIIDirectory.h"
#include "..\NTDLL\include\NTDLL.h"

#include "ThreadsRecoverStage.h"

#include "..\URSDriver\include\URSDriverInterface.h"

#pragma comment(lib, "URSDriver.lib")
#pragma comment(lib, "NTDLL.lib")
#pragma comment(lib, "RAII.lib")


bool URS::Init(
	_In_ DWORD PID,
	_In_ DebuggerFunctions Functions) {

	m_PID = PID;
	m_Functions = Functions;

	m_ProcessState.RegisterRecoverStage(std::make_shared<ThreadsRecoverStage>());


	return true;
}

bool URS::PostProcessInit() {

	bool res = true;
	
	do {
		res = NTDLL::GetInstance().Init();
		if (!res) {
			ERROR_LOG("Failed to init ntdll");
			break;
		}

		res = TargetProcess::GetInstance().Init(m_PID);
		if (!res) {
			ERROR_LOG("Failed to init target process");
			break;
		}

		res = m_URSDLL.Load();
		if (!res) {
			ERROR_LOG("Failed to load ursdll");
			break;
		}

		res = URSDriverInit();
		if (!res) {
			ERROR_LOG("Failed to init urs driver");
			break;
		}

		

	} while (false);

	return res;
}

bool URS::RecoverMappedMemPages(_In_ std::shared_ptr<RAIIProcess> TargetProcess) {
	bool res = true;
	MEMORY_BASIC_INFORMATION info;
	HANDLE process =
		TargetProcess::GetInstance().GetProcess()->GetRAIIHandle()->GetHandle();

	do {


		ULONG64 addr = 0;

		while (VirtualQueryEx(
			process,
			(LPCVOID)addr,
			&info,
			sizeof(info))) {

			if (info.Type == MEM_MAPPED) {
				auto addr = VirtualAllocEx(
					TargetProcess->GetRAIIHandle()->GetHandle(),
					info.BaseAddress,
					info.RegionSize, MEM_COMMIT | MEM_RESERVE,
					PAGE_READWRITE
				);

				if (addr != nullptr) {
					void* buffer = malloc(info.RegionSize);
					res = ReadProcessMemory(
						process,
						info.BaseAddress,
						buffer,
						info.RegionSize,
						nullptr
					);
					if (!res) {
						ERROR_LOG("Failed to read process mem");
						break;
					}


					WriteProcessMemory(
						TargetProcess->GetRAIIHandle()->GetHandle(),
						info.BaseAddress,
						buffer,
						info.RegionSize,
						nullptr
					);
				}
			}

			addr += info.RegionSize;
		}
	} while (false);

	return res;
}

bool URS::CaptureState() {
	bool res = true;

	do {
		res = TargetProcess::GetInstance().GetProcess()->Suspend();
		if (!res) {
			ERROR_LOG("Failed to suspend process");
			break;
		}
		res = m_ProcessState.Capture();
		if (!res) {
			ERROR_LOG("Failed to capture process state");
			break;
		}

		DWORD cloneId;
		res = m_URSDLL.CloneTargetProcess(cloneId);
		if (!res) {
			ERROR_LOG("Failed to clone target process");
			break;
		}

		m_CloneOfProcess = std::make_shared<RAIIProcess>();
		res = m_CloneOfProcess->Open(cloneId);
		if (!res) {
			ERROR_LOG("Failed to open clone process");
			break;
		}

		/*
		* Can 



		*/
		RecoverMappedMemPages(m_CloneOfProcess);

		res = TargetProcess::GetInstance().GetProcess()->Resume();
		if (!res) {
			ERROR_LOG("Failed to resume");
			break;
		}
	} while (false);

	return res;
}

bool URS::RecoverState() {
	bool res = true;
	
	do {

		

		res = m_Functions.DetachDebugger();
		if (!res) {
			ERROR_LOG("Failed to detach debugger");
			break;
		}

		res = TargetProcess::GetInstance().GetProcess()->Terminate();
		if (!res) {
			ERROR_LOG("Failed to terminate old process");
			break;
		}
		TargetProcess::GetInstance().SetProcess(m_CloneOfProcess);

		res = m_ProcessState.RecoverState();
		if (!res) {
			ERROR_LOG("Failed to recover state");
			break;
		}

		res = m_Functions.ReatachDebugger(m_CloneOfProcess);
		if (!res) {
			ERROR_LOG("Failed to reatach debugger");
		}



	} while (false);

	return res;
}

bool URS::CompareSnapshots(_Out_ bool& Equals) {
	bool res = true;

	do {
		ERROR_LOG("Not implemented");
		Equals = false;
	} while (false);

	return res;
}