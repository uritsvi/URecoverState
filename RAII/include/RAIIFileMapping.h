#pragma once

#include "RAIIFile.h"

#include <memory>

class RAIIFileMapping {

public:
	RAIIFileMapping() = default;
	RAIIFileMapping(
		_In_ std::shared_ptr<RAIIFile> File,
		_In_ ULONG Size
	);
	RAIIFileMapping(
		_In_ RAIIFile& File,
		_In_ ULONG Size
	);

	void Map(_In_ HANDLE File,
			 _In_ ULONG Size
	);

	~RAIIFileMapping();

	LPVOID GetMappedAddr();

private:
	LPVOID m_MappedAddr;
	HANDLE m_FileMapping;
};