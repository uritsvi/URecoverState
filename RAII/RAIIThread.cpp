#include "include\RAIIThread.h"

RAIIThread::RAIIThread() {
	m_Handle = nullptr;
}
RAIIThread::~RAIIThread() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

bool RAIIThread::Open(DWORD Tid) {
	bool res = true;

	do {
		m_Handle = OpenThread(
			THREAD_ALL_ACCESS,
			FALSE,
			Tid
		);
		if (m_Handle == INVALID_HANDLE_VALUE) {
			res = false;
			break;
		}
	} while (false);

	return res;
	
}

bool RAIIThread::Create(
	_In_ LPTHREAD_START_ROUTINE Function,
	_In_ LPVOID Ctx,
	_Out_ DWORD& Tid) {
	bool res = true;

	do {
		res = CreateThread(
			nullptr, 
			0, 
			Function, 
			Ctx, 
			0, 
			&Tid
		);
		if (!res) {
			break;
		}
	} while (false);

	return res;
}


bool RAIIThread::SetContext(_In_ CONTEXT& Context) {
	bool res = true;
	
	do {
		res = SetThreadContext(
			m_Handle,
			&Context
		);
		if (!res) {
			break;
		}
	} while (false);

	return res;
}

bool RAIIThread::Resume() {
	if (m_Handle == nullptr) {
		return false;
	}

	return ResumeThread(m_Handle);
}
