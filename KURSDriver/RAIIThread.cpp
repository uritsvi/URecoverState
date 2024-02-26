#include <ntifs.h>


#include "RAIIThread.h"

RAIIThread::RAIIThread() {
	m_Thread = nullptr;
	m_Id = nullptr;
}
RAIIThread::~RAIIThread() {
	if (m_Thread != nullptr) {
		ObDereferenceObject(m_Thread);
	}
}

NTSTATUS RAIIThread::Open(HANDLE Id) {

	NTSTATUS status = STATUS_SUCCESS;

	do {
		m_Id = Id;

		status = PsLookupThreadByThreadId(
			m_Id,
			(PETHREAD*)&m_Thread
		);
		if (!NT_SUCCESS(status)) {

			KdPrint(("failed to find target ethread"));
			break;
		}
	} while (false);

	return status;
}

HANDLE RAIIThread::GetId() {
	return m_Id;
}

PVOID RAIIThread::GetEthread() {
	return m_Thread;
}