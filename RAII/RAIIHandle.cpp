#include "include\RAIIHandle.h"
#include "include\RAIIProcess.h"

RAIIHandle::RAIIHandle(){
}
RAIIHandle::RAIIHandle(HANDLE Handle) {
	m_Handle = Handle;
}
RAIIHandle::~RAIIHandle() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

HANDLE RAIIHandle::GetHandle() {
	return m_Handle;
}
bool RAIIHandle::DupHandle(
	_In_ std::shared_ptr<RAIIProcess> Target, 
	_Out_ HANDLE& Out) {

	return DuplicateHandle(
		GetCurrentProcess(),
		m_Handle, 
		Target->GetRAIIHandle()->GetHandle(), 
		&Out, 
		GENERIC_ALL, 
		FALSE, 
		DUPLICATE_CLOSE_SOURCE
	);
}