#pragma once


#include "RAII/include/RAIIFile.h"
#include "RAII/include/RAIIFileMapping.h"
#include "RAII/include/RAIIDirectory.h"

#include <DbgHelp.h>


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
	bool Dump(_In_ std::shared_ptr<RAIIDirectory> TargetDirectory);
	bool Read(_In_ StateInfo& Info);

private:
	RAIIFile m_File;
};

