#pragma once

typedef std::pair<ULONG64, PMINIDUMP_MEMORY_INFO> MemInfoPair;
typedef std::unordered_map<ULONG64, PMINIDUMP_MEMORY_INFO> MemInfoDict;

/*
#include "CSVBuffer.h"
#include "RAII/include/RAIIBuffer.h"

#include <DbgHelp.h>

#include <memoryapi.h>
#include <list>
#include <unordered_map>
#include <memory>


#pragma comment(lib, "Onecore.lib")




class MemAllocInfo
{
public:
	MemAllocInfo();
	~MemAllocInfo();

	bool Dump(
		_In_ MemInfoDict& Dict
	);
	bool FindInfo(
		_In_ ULONG64 RangeStart, 
		_In_ UINT64 EndAddr, 
		_Out_ std::list<std::shared_ptr<WIN32_MEMORY_REGION_INFORMATION>>& Regions
	);

private:
	ULONG64 m_LastSize;
	CSVBuffer m_CsvBuffer;

	RAIIBuffer m_RegionBuffers;
};

*/