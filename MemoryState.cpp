#include "MemoryState.h"
#include "Error.h"

std::string MemToString(PMINIDUMP_MEMORY_INFO& Info) {
	std::string out = "";
	out += "AllocationBase: " + std::to_string(Info->BaseAddress) + "\n";
	out += "AllocationBase: " + std::to_string(Info->AllocationBase) + "\n";
	out += "AllocationProtect: " + std::to_string(Info->AllocationProtect) + "\n";
	out += "RegionSize: " + std::to_string(Info->RegionSize) + "\n";
	out += "State: " + std::to_string(Info->State) + "\n";
	out += "Protect: " + std::to_string(Info->Protect) + "\n";
	out += "Type: " + std::to_string(Info->Type) + "\n";

	return out;
}

MemoryState::MemoryState() {

}
MemoryState::~MemoryState() {

}

bool MemoryState::Init(_In_ StateInfo& State) {
	bool res = true;

	do {
		res = InitMemInfoList(State);
		if (!res) {
			ERROR_LOG("Failed to init mem info list");
			break;
		}

		res = m_AllocationsState.Init(
			this,
			State
		);
		if (!res) {
			ERROR_LOG("Failed to init allocations state");
			break;
		}
	} while (false);

	return res;
}

bool MemoryState::Revert(MemoryState& Current) {
	bool res = true;

	do {
		res = m_AllocationsState.Revert(
			Current.m_AllocationsState
		);
		if (!res) {
			ERROR_LOG("Failed to revert allocations state");
			break;
		}
	} while (false);

	return res;
}

bool MemoryState::GetMemInfo(
	_In_ UINT64 Addr,
	_Out_ PMINIDUMP_MEMORY_INFO& Info) {

	for (auto entry : m_MemInfoList) {
		
		auto endAddr = entry->BaseAddress + entry->RegionSize;
		
		if (Addr >= entry->BaseAddress && Addr <= endAddr) {
			Info = entry;
			return true;
		}
	}

	return false;
}

bool MemoryState::InitMemInfoList(_In_ StateInfo& State) {
	bool res = true;

	do {
		auto memInfo = State.MemInfo;
		auto first = (PMINIDUMP_MEMORY_INFO)&memInfo[1];

		for (int i = 0; i < memInfo->NumberOfEntries; i++) {
			auto current = &first[i];
			m_MemInfoList.push_back(current);
		}

	} while (false);

	return res;
}