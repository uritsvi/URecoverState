#include "../RAII/include/RAIIBuffer.h"

RAIIBuffer::RAIIBuffer(ULONG64 Size) {
	m_Buffer = nullptr;
	m_Buffer = malloc(Size);
	m_Size = Size;
}
RAIIBuffer::~RAIIBuffer() {
	free(m_Buffer);
}

LPVOID RAIIBuffer::GetPtr() {
	return m_Buffer;
}

ULONG64 RAIIBuffer::GetSize() {
	return m_Size;
}
