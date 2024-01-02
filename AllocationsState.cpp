#include "AllocationsState.h"
#include "MemoryState.h"
#include "Error.h"
#include "TargetProcess.h"

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
			allocSize,
			memInfo
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


	/*
	* GetAllocsInRange: regionBase, size, out allocs
	* 
	* end = regionBase + size
	* 
	* for entry in allocations
	*	if end > entry.base && end <= entry.end
	*		allocs.push(entry)
	*/
	auto allocEnd = Base + Size;

	for (auto entry : m_Allocations) {
		auto base = entry.first;
		auto end = entry.first + entry.second->Size;

		auto current = entry.second;

		if (allocEnd > base && allocEnd <= (current->AllocBase + current->Size)) {
			Allocs.push_back(entry.second);
		}
	}

	if (Allocs.size() == 1) {
		
		auto alloc = Allocs[0];
		if (alloc->AllocBase == Base && alloc->Size == Size) {
			SameAlloc = true;
		}

	}

	return true;
}

bool AllocationsState::Revert(_In_ AllocationsState& Current) {
	bool res = true;

	do {
		for(auto& region : m_Allocations){
			auto currentRegion = region.second;
			bool sameAlloc = false;

			std::vector<std::shared_ptr<AllocationInfo>> allocsInRange;

			res = Current.GetAllocsInRange(
				currentRegion->AllocBase, 
				currentRegion->Size, 
				sameAlloc, 
				allocsInRange
			);
			if (!res) {
				ERROR_LOG("Failed to get allocs info");
				break;
			}

			if (sameAlloc) {
				DWORD old;
				res = VirtualProtect(
					(LPVOID)currentRegion->AllocBase, 
					currentRegion->Info->RegionSize, 
					currentRegion->Info->Protect, 
					&old
				);
				if (!res) {
					ERROR_LOG("Failed to protect current region");
					break;
				}
				continue;
			}
			
			res = VirtualFreeEx(
				TargetProcess::GetInstance().GetProcess(), 
				(LPVOID)currentRegion->AllocBase, 
				0, 
				MEM_FREE
			);
			if (!res) {
				ERROR_LOG("Failed to free virtual memory");
				break;
			}
			for (auto& oldRegion : allocsInRange) {
				res = VirtualAllocEx(
					TargetProcess::GetInstance().GetProcess(), 
					(LPVOID)oldRegion->AllocBase, 
					oldRegion->Size, 
					oldRegion->Info->State | MEM_RESERVE, 
					oldRegion->Info->Protect
				);
				if (!res) {
					ERROR_LOG("Failed to allocate virtual memory");
					break;
				}
			}
		}
	} while (false);

	return res;
}