/*
#include "MemAllocInfo.h"
#include "RAII/include/RAIIFileMapping.h"
#include "TargetProcess.h"
#include "Error.h"

#define CSV_TOP_COLOMNS "BASE, END, INFO"

void _MakeRow(
	_In_ ULONG64 StartAddr, 
	_In_ ULONG64 EndAddr,
	_In_ int InfoIndex,
	_Out_ std::string& Row) {

	Row = std::to_string(StartAddr) + 
		"," + 
		std::to_string(EndAddr) + 
		"," + 
		std::to_string(InfoIndex);
}

MemAllocInfo::MemAllocInfo() {
}
MemAllocInfo::~MemAllocInfo() {
}

bool MemAllocInfo::Dump(_In_ MemInfoDict& Dict) {

	bool res = true;

	do {

		m_CsvBuffer.PushRow(CSV_TOP_COLOMNS);
		int i = 0;
		std::list<WIN32_MEMORY_REGION_INFORMATION> infoBuffers;

		for (auto& region : Dict) {
			
			WIN32_MEMORY_REGION_INFORMATION info = {0};
			
			auto currentAddr = region.first;
			ULONG64 endAddr = currentAddr + region.second->RegionSize;


			do {
	

				res = QueryVirtualMemoryInformation(
					TargetProcess::GetInstance().GetProcess(),
					(LPVOID)currentAddr,
					MemoryRegionInfo,
					&info,
					sizeof(info),
					nullptr
				);
				if (!res) {
					ERROR_LOG("Failed to query alloc info");
					break;
				
				}

				auto allocationEnd =
					currentAddr += info.RegionSize;

				std::string row;
				_MakeRow(
					currentAddr, 
					allocationEnd, 
					i++, 
					row
				);
				m_CsvBuffer.PushRow(row);

				infoBuffers.push_back(info);

				currentAddr = allocationEnd;

			} while (currentAddr < endAddr);

		}
	

		m_RegionBuffers = RAIIBuffer(
			infoBuffers.size() * 
			sizeof(WIN32_MEMORY_REGION_INFORMATION)
		);
		auto ptr = (ULONG64)m_RegionBuffers.GetPtr();
		if (ptr == 0) {
			ERROR_LOG("Failed to allocate buffer");
			break;
		}
		for (auto info : infoBuffers) {
			memcpy(
				(LPVOID)ptr,
				&info,
				sizeof(info)
			);

			ptr + sizeof(info);
		}




	} while (false);

	return res;
}
bool MemAllocInfo::FindInfo(
	_In_ ULONG64 RangeStart,
	_In_ UINT64 EndAddr,
	_Out_ std::list<std::shared_ptr<WIN32_MEMORY_REGION_INFORMATION>>& Regions) {

	bool res = true;
	
	do {
		for (int i = 0; i < 0; i++) {
			std::string indexStr;
			m_CsvBuffer.GetByIndex(
				0, 
				i, 
				indexStr
			);

			int index = std::stoi(indexStr);
		
		}

		auto ptr = (ULONG64)m_RegionBuffers.GetPtr();
		if (ptr == 0) {
			ERROR_LOG("Regions buffers is nullptr");
			break;
		}

		for (ULONG64 i = 0; 
			i < m_RegionBuffers.GetSize();
			i += sizeof(WIN32_MEMORY_REGION_INFORMATION)) {
			
			auto region = (WIN32_MEMORY_REGION_INFORMATION*)(ptr + i);
			if((ULONG64)region->AllocationBase >= RangeStart){
			}

		}

	} while (false);

	return res;
}
*/