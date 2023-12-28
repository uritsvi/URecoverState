#include "../include/RAIIFileMapping.h"

RAIIFileMapping::RAIIFileMapping(
	_In_ std::shared_ptr<RAIIFile> File,
	_In_ ULONG Size) {

	Map(File->GetHandle(), Size);
}
RAIIFileMapping::RAIIFileMapping(
	_In_ RAIIFile& File,
	_In_ ULONG Size) {

	Map(File.GetHandle(), Size);

}

RAIIFileMapping::~RAIIFileMapping() {
}

void RAIIFileMapping::Map(
	_In_ HANDLE File,
	_In_ ULONG Size) 
{

	m_MappedAddr = nullptr;

	m_FileMapping = CreateFileMappingA(
		File,
		nullptr,
		PAGE_EXECUTE_READWRITE,
		0,
		0,
		nullptr
	);
	if (m_FileMapping == INVALID_HANDLE_VALUE) {
		return;
	}

	m_MappedAddr = MapViewOfFile(
		m_FileMapping,
		FILE_ALL_ACCESS,
		0,
		0,
		Size
	);
	if (m_MappedAddr == nullptr) {
		return;
	}
}

LPVOID RAIIFileMapping::GetMappedAddr() {
	return m_MappedAddr;
}