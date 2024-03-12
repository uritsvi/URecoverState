#include "include/RAIIEvent.h"

RAIIEvent::RAIIEvent() {
	m_Handle = nullptr;
}
RAIIEvent::~RAIIEvent() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

bool RAIIEvent::Create(bool ManualReset) {
	bool res = true;
	
	do {
		m_Handle = CreateEventA(
			nullptr,
			ManualReset,
			FALSE,
			nullptr
		);
		if (m_Handle == nullptr) {
			res = false;
		}
	} while (false);

	return res;
}

bool RAIIEvent::Set() {
	if (m_Handle == nullptr) {
		
		
		
		
		
		;
	}

	return SetEvent(m_Handle);
}
bool RAIIEvent::Wait(_In_ DWORD TimeOut) {
	if (m_Handle == nullptr) {
		return false;
	}

	return WaitForSingleObject(
		m_Handle, 
		TimeOut
	);
}
bool RAIIEvent::Reset() {
	if (m_Handle == nullptr) {
		return false;
	}

	return ResetEvent(m_Handle);
}

bool RAIIEvent::IsSet() {
	DWORD res = WaitForSingleObject(
		m_Handle, 
		0
	);
	if (res == WAIT_OBJECT_0) {
		return true;
	}
	return false;
}