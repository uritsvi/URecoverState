#include "include/RAIIProcess.h"

RAIIProcess::RAIIProcess() {
	m_Handle = nullptr;
	m_PID = 0;
}
RAIIProcess::~RAIIProcess() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

bool RAIIProcess::Create(
	_In_ std::string EXE,
	_In_ std::string Cmd,
	_In_ ULONG Flags) {

	bool res = true;

	do {
		std::string cmd = EXE + " " + Cmd;

		STARTUPINFOA info = {0};
		info.cb = sizeof(info);

		PROCESS_INFORMATION processInfo = {0};

		res = CreateProcessA(
			EXE.c_str(),
			(LPSTR)cmd.c_str(),
			nullptr,
			nullptr,
			FALSE,
			Flags,
			nullptr,
			nullptr, 
			&info,
			&processInfo
		);
		if (!res) {
			break;
		}

		m_Handle = processInfo.hProcess;
		m_PID = processInfo.dwProcessId;
	} while (false);

	return res;
}
bool RAIIProcess::Open(_In_ DWORD PID) {
	bool res = true;

	do {
		m_PID;
		m_Handle = OpenProcess(
			PROCESS_ALL_ACCESS, 
			FALSE, 
			PID
		);
		if (m_Handle == nullptr) {
			res = false;
			break;
		}

	} while (false);

	return res;
}

DWORD RAIIProcess::GetPID() {
	return m_PID;
}

HANDLE RAIIProcess::GetHandle() {
	return m_Handle;
}

