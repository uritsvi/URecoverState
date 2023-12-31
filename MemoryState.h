#pragma once

#include "DmpFile.h"
#include "AllocationsState.h"

#include <list>


std::string MemToString(PMINIDUMP_MEMORY_INFO& Info);

class MemoryState
{
public:
	MemoryState();
	~MemoryState();

	bool Init(_In_ StateInfo& State);

	bool Revert(MemoryState& Current);

	/*
	* This function is public because it's in use by the 
	* AllocationsState class
	*/
	bool GetMemInfo(
		_In_ UINT64 Addr, 
		_Out_ PMINIDUMP_MEMORY_INFO& Info
	);

private:
	bool InitMemInfoList(_In_ StateInfo& State);

private:
	AllocationsState m_AllocationsState;
	std::list<PMINIDUMP_MEMORY_INFO> m_MemInfoList;
};

