#include "include/RAIIFileMapping.h"



bool RAIIFileMapping::Create(
	_In_ RAIIFile& File,
	_In_ ULONG Size) {

	return Map(
		File.GetHandle(), 
		Size
	);
}


RAIIFileMapping::~RAIIFileMapping() {
	if (m_MappedAddr != nullptr) {
		UnmapViewOfFile(m_MappedAddr);

	}

}

bool RAIIFileMapping::Map(
	_In_ HANDLE File,
	_In_ ULONG Size) 
{

	bool res = true;

	do {
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
			res = false;
			break;
		}

		m_MappedAddr = MapViewOfFile(
			m_FileMapping,
			FILE_ALL_ACCESS,
			0,
			0,
			Size
		);
		if (m_MappedAddr == nullptr) {
			res = false;
			break;
		}
	} while (false);

	return res;
}

LPVOID RAIIFileMapping::GetMappedAddr() {
	return m_MappedAddr;
}