#include "RAIIBuffer.h"
#include "kmemory.h"

RAIIBuffer::RAIIBuffer() {
	m_BufferOwner = false;

}

RAIIBuffer::RAIIBuffer(SIZE_T Size) {
	
	m_BufferOwner = true;

	m_Buffer = ExAllocatePool2(
		POOL_FLAG_PAGED, 
		Size, 
		DRIVER_TAG
	);
	if (m_Buffer == nullptr) {
		KdPrint(("Failed to allocate mem"));
	}
}
RAIIBuffer::~RAIIBuffer() {
	if (m_Buffer == nullptr || !m_BufferOwner) {
		return;
	}
	ExFreePool(m_Buffer);

}

PVOID64 RAIIBuffer::GetAddr() {
	return m_Buffer;
}

NTSTATUS RAIIBuffer::Move(_In_ RAIIBuffer& Buffer) {

	NTSTATUS status = STATUS_SUCCESS;

	do {
		if (Buffer.m_BufferOwner) {
			KdPrint(("Failed to move, buffer is allready an owner"));
			status = STATUS_UNSUCCESSFUL;
		}

		m_BufferOwner = false;

		Buffer.m_Buffer = m_Buffer;
		Buffer.m_BufferOwner = true;

	} while (false);

	return status;

}