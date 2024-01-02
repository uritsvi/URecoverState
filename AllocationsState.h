#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "DmpFile.h"

class MemoryState;

struct AllocationInfo {

	AllocationInfo(ULONG64 Base, ULONG64 Size) {
		this->Size = Size;
		this->Base = Base;
	}

	ULONG64 Size;
	ULONG64 Base;
};

typedef std::pair<ULONG64, std::shared_ptr<AllocationInfo>> AllocationPair;

class AllocationsState
{
public:
	AllocationsState();
	~AllocationsState();

	bool Init(
		_In_ MemoryState* MemState, 
		_In_ StateInfo& CurrentState
	);
	bool Revert(_In_ AllocationsState& Current);

	bool GetAllocsInRange(
		_In_ UINT64 Base, 
		_In_ UINT64 Size,
		_Out_ bool& SameAlloc,
		_Out_ std::vector<std::shared_ptr<AllocationInfo>> Allocs
	);

private:
	std::unordered_map<ULONG64, std::shared_ptr<AllocationInfo>> m_Allocations;
};

