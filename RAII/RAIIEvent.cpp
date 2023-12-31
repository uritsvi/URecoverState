#include "../include/RAIIEvent.h"

RAIIEvent::RAIIEvent() {
	m_Handle = nullptr;
}
RAIIEvent::~RAIIEvent() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

bool RAIIEvent::Create() {
	bool res = true;
	
	do {
		m_Handle = CreateEvent(
			nullptr,
			TRUE,
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
		return false;
	}

	return SetEvent(m_Handle);
}
bool RAIIEvent::Wait() {
	if (m_Handle == nullptr) {
		return false;
	}

	return WaitForSingleObject(
		m_Handle, 
		FALSE
	);
}
bool RAIIEvent::Reset() {
	if (m_Handle == nullptr) {
		return false;
	}

	return ResetEvent(m_Handle);
}