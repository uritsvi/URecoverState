#include <ntifs.h>

#include "RAIIProcess.h"

RAIIProcess::RAIIProcess() {
	m_Process = nullptr;
	m_Id = 0;
}
RAIIProcess::~RAIIProcess() {
	if (m_Process != nullptr) {
		ObDereferenceObject(m_Process);
	}
}

NTSTATUS RAIIProcess::Open(HANDLE Id) {
	m_Id = Id;
	
	return PsLookupProcessByProcessId(
		Id, 
		&m_Process
	);
}

PVOID RAIIProcess::GetEProcess() {
	return m_Process;
}

HANDLE RAIIProcess::GetId() {
	return m_Id;
}