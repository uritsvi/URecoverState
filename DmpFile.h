#pragma once

#include "RAII/include/RAIIFile.h"
#include "RAII/include/RAIIFileMapping.h"

struct StateInfo {
	RAIIFileMapping Mapping;

	PMINIDUMP_THREAD_LIST ThreadInfo;
	PMINIDUMP_MEMORY_INFO_LIST MemInfo;
	PMINIDUMP_MEMORY64_LIST Mem;
	PMINIDUMP_HANDLE_DATA_STREAM HandlesInfo;
	PMINIDUMP_MODULE_LIST ModulesInfo;
};

class DmpFile
{
public:
	bool Dump();
	bool Read(_In_ StateInfo& Info);

	bool ReadMiniDumpStream(
		_In_ MINIDUMP_STREAM_TYPE, 
		_Out_ LPVOID Addr
	);

private:
	RAIIFile m_File;
};

