#include "URSDriver.h"
#include "..\Common\include\Error.h"


#include "..\RAII\include\RAIIProcess.h"
#include "..\RAII\include\RAIIThread.h"
#include "../Common/include/Common.h"


#define KDMAPPER_EXE_NAME "kdmapper.exe"
#define DRIVER_FILE_NAME "KURSDriver.sys"

#define SYM_LINK "\\\\.\\KURS"

bool URSDriver::Init() {
	bool res = true;

	do {
		res = OpenDriver();
		if (!res) {
			
			res = LoadDriver();
			if (!res) {
				ERROR_LOG("Failed to load driver");
				break;
			}

			res = OpenDriver();
			if (!res) {
				ERROR_LOG("Failed to open driver");
				break;
			}

			ERROR_LOG("Failed to open driver");
			break;
		}
		res = LoadKernelSymbols();
		if (!res) {
			ERROR_LOG("Failed to load kernel symbols");
			break;
		}



	} while (false);


	return res;
}

bool URSDriver::LoadKernelSymbols() {

	bool res = true;

	do {

		KernelInfo kernelInfo;
		res = GetBaseNTOSKRNL(kernelInfo);
		if (!res) {
			ERROR_LOG("Failed to get kernel info");
			break;
		}

		res = m_Symbols.Init(
			kernelInfo
		);
		if (!res) {
			ERROR_LOG("Failed to init symbols");
			break;
		}

		KernelFunctions symbols;
		res = m_Symbols.FindAllSymbols(symbols);
		if (!res) {
			ERROR_LOG("Failed to find symbols");
			break;
		}

		res = PassSymbolsToDriver(symbols);
		if (!res) {
			ERROR_LOG("Failed to pass symbols to driver");
			break;
		}

	} while (false);

	return res;
}

bool URSDriver::OpenDriver() {
	bool res = true;

	do {
		m_DriverHandle = CreateFileA(
			SYM_LINK, 
			GENERIC_READ | GENERIC_WRITE, 
			0, 
			0, 
			OPEN_EXISTING, 
			0, 
			0
		);
		if (m_DriverHandle == INVALID_HANDLE_VALUE) {
			res = false;
			break;
		}
	} while (false);

	return res;
}

bool URSDriver::LoadDriver() {

	bool res = true;

	do {
		char driverFullPath[MAX_PATH];
		res = GetFullPathNameA(
			DRIVER_FILE_NAME,
			sizeof(driverFullPath),
			driverFullPath,
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to get driver full path");
			break;
		}

		RAIIProcess kdmapper;
		res = kdmapper.Create(
			KDMAPPER_EXE_NAME,
			driverFullPath,
			0
		);
		if (!res) {
			ERROR_LOG("Failed to create kdmapper process");
			break;
		}
		DWORD exitCode;
		res = kdmapper.WaitForExit(exitCode);
		if (!res || exitCode != 0) {
			res = false;
			ERROR_LOG("kdmapper failed");
			break;
		}
		std::cout << "Sleep\n";

	} while (false);

	return res;
}

bool URSDriver::GetBaseNTOSKRNL(_Out_ KernelInfo& Addr) {
	bool res = true;

	do {
		res = DeviceIoControl(
			m_DriverHandle, 
			IOCTL_GET_KERNEL_BASE, 
			nullptr, 
			0, 
			&Addr, 
			sizeof(KernelInfo), 
			nullptr,
			0);
		if (!res) {
			ERROR_LOG("Failed to get kernel base");
			break;
		}
	} while (false);

	return res;

}
bool URSDriver::PassSymbolsToDriver(_In_ KernelFunctions& Symbols) {
	bool res = true;

	do {

		res = DeviceIoControl(
			m_DriverHandle, 
			IOCTL_LOAD_SYMBOLS, 
			&Symbols, 
			sizeof(KernelFunctions), 
			nullptr, 
			0, 
			nullptr, 
			0
		);
		if (!res) {
			ERROR_LOG("Failed to load kernel symbols");
			break;
		}

	} while (false);

	return res;
}

bool URSDriver::ChangeThreadTEB(
	_In_ DWORD Tid, 
	_In_ LPVOID NewTEB) {
	
	bool res = true;

	do {
		ChangeThreadTEBParams params = {
			(HANDLE)Tid, 
			NewTEB
		};

		res = DeviceIoControl(
			m_DriverHandle,
			IOCTL_CHANGE_THREAD_TEB,
			&params,
			sizeof(ChangeThreadTEBParams),
			nullptr,
			0,
			nullptr,
			0
		);
		if (!res) {
			ERROR_LOG("Failed to move thread id");
			break;
		}

		
	} while (false);

	return res;
}

bool URSDriver::MoveThreadId(
	_In_ DWORD Target,
	_In_ DWORD Src) {

	bool res = true;

	do {
		/*
		RAIIThread srcThread;
		res = srcThread.Open(Src);
		if (!res) {
			ERROR_LOG("Failed to open src thread");
			break;
		}
		*/
		/*
		res = srcThread.Suspend();
		if (!res) {
			ERROR_LOG("Failed to suspend thread");
			break;
		}
		*/

		MoveThreadIdParams params = {0};

		params.SrcId = (HANDLE)Src;
		params.TargetId = (HANDLE)Target;

		res = DeviceIoControl(
			m_DriverHandle,
			IOCTL_MOVE_THREAD_ID,
			&params,
			sizeof(MoveThreadIdParams),
			nullptr,
			0,
			nullptr,
			0
		);
		if (!res) {
			ERROR_LOG("Failed to move thread id");
			break;
		}
	} while (false);
	
	return res;
}

bool URSDriver::ChangeProcessId(
	_In_ DWORD Pid,
	_In_ DWORD TargetId) {

	bool res = true;

	do {

		ChangeProcessIdParams params = { 0 };

		params.ProcessId = (HANDLE)Pid;
		params.TargetId = (HANDLE)TargetId;

		res = DeviceIoControl(
			m_DriverHandle,
			IOCTL_CHANGE_PROCESS_ID,
			&params,
			sizeof(ChangeProcessIdParams),
			nullptr,
			0,
			nullptr,
			0
		);
		if (!res) {
			ERROR_LOG("Failed to change process id");
			break;
		}
	} while (false);

	return res;
}

bool URSDriver::ChangeHandle(
	_In_ HANDLE SrcHandle,
	_In_ HANDLE TargetHandle,
	_In_ DWORD Pid) {

	bool res = true;

	do {

		ChangeHandleParams params = { 0 };

		params.SrcHandle = SrcHandle;
		params.TargetHandle= TargetHandle;
		params.Pid = (HANDLE)Pid;

		res = DeviceIoControl(
			m_DriverHandle,
			IOCTL_CHANGE_HANDLE,
			&params,
			sizeof(ChangeHandleParams),
			nullptr,
			0,
			nullptr,
			0
		);
		if (!res) {
			ERROR_LOG("Failed to change handle");
			break;
		}
	} while (false);

	return res;
}

