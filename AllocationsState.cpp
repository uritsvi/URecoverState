#include "AllocationsState.h"
#include "MemoryState.h"
#include "Error.h"

#define PAGE_SIZE (1 << 12)

AllocationsState::AllocationsState() {

}
AllocationsState::~AllocationsState() {

}

bool AllocationsState::Init(
	_In_ MemoryState* MemState,
	_In_ StateInfo& CurrentState) {

	bool res = true;

	UINT64 i = 0;
	PMINIDUMP_MEMORY_INFO memInfo;
	
	while (true) {
		res = MemState->GetMemInfo(
			i, 
			memInfo
		);
		if (!res) {
			ERROR_LOG("Failed to get mem info");
			break;
		}

		auto allocBase = memInfo->AllocationBase;
		if (m_Allocations.find(allocBase) != m_Allocations.end()) {
			continue;
		}

		bool canotBeAllocated = false;
		if (memInfo->State == MEM_FREE) {
			canotBeAllocated = true;
		}
		else if (memInfo->Type == MEM_IMAGE || memInfo->Type == MEM_MAPPED) {
			WARNING_LOG("Mapped memory:");
			WARNING_LOG(MemToString(memInfo));

			canotBeAllocated = true;
		}

		if (canotBeAllocated) {
			i += memInfo->RegionSize;
			continue;
		}

		auto allocSize = 
			(allocBase - memInfo->BaseAddress) + 
			memInfo->RegionSize;

		auto allocInfo = std::make_shared<AllocationInfo>(
			allocBase, 
			allocSize
		);
		m_Allocations.insert(AllocationPair(
			allocBase, 
			allocInfo)
		);

		i += PAGE_SIZE;
	}

	return res;
}

bool AllocationsState::GetAllocsInRange(
	_In_ UINT64 Base,
	_In_ UINT64 Size,
	_Out_ bool& SameAlloc,
	_Out_ std::vector<std::shared_ptr<AllocationInfo>> Allocs) {

	auto allocEnd = Base + Size;

	for (auto entry : m_Allocations) {
		auto base = entry.first;
		auto end = entry.first + entry.second->Size;

		if (Base >= base || allocEnd <= end) {
			Allocs.push_back(entry.second);
		}
	}

	if (Allocs.size() == 1) {
		
		auto alloc = Allocs[0];
		if (alloc->Base == Base && alloc->Size == Size) {
			SameAlloc = true;
		}

	}

	return true;
}

bool AllocationsState::Revert(_In_ AllocationsState& Current) {
	bool res = true;

	do {

	} while (false);

	return res;
}