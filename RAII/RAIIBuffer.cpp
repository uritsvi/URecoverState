#include "../RAII/include/RAIIBuffer.h"

RAIIBuffer::RAIIBuffer() {
}
RAIIBuffer::~RAIIBuffer() {
	free(m_Buffer);
}

bool RAIIBuffer::Create(ULONG64 Size) {
	m_Buffer = malloc(Size);
	if (m_Buffer == nullptr) {
		return false;
	}

	m_Size = Size;

	return true;
}

LPVOID RAIIBuffer::GetPtr() {
	return m_Buffer;
}

ULONG64 RAIIBuffer::GetSize() {
	return m_Size;
}
