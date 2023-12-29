#pragma once

#include "RAIIFile.h"

#include <memory>

class RAIIFileMapping {

public:
	RAIIFileMapping() = default;
	~RAIIFileMapping();

	bool Create(
		_In_ RAIIFile& File,
		_In_ ULONG Size
	);

	LPVOID GetMappedAddr();


private:
	bool Map(_In_ HANDLE File,
			 _In_ ULONG Size
	);

private:
	LPVOID m_MappedAddr;
	HANDLE m_FileMapping;
};