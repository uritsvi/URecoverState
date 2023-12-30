#include <locale>
#include <codecvt>

#include "ProcessState.h"
#include "TargetProcess.h"
#include "Error.h"
#include "RAII/include/RAIIThread.h"
#include "RAII/include/RAIIFileMapping.h"

#include <unordered_map>
#include <minidumpapiset.h>

#pragma comment(lib, "Dbghelp.lib")

/*
* This error is in use more thene one time
*/
#define THREADS_MISS_MATCH_ERROR "Threads miss match, Some threads have been created or destroied not supported"

#define FAILED_TO_MAP_DUMP_FILE "Failed to map dump file to memory"
#define FAILED_TO_READ_DUMP_FILE "Failed to read dump file state"
#define FAILED_TO_UNMAP_FILE_FROM_MEMORY "Failed to unmap dump file from memory"

#define NON_PRIVATE_NEM_NOT_SUPPORTED "Not supported dealocating non private mem"


/*
ProcessState& ProcessState::GetInstance() {
	static ProcessState state = ProcessState();
	return state;
}
*/

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

/*
bool ProcessState::Init() {

	StateFiles dummy;
	bool res = dummy.Create("dummy", true);
	if (!res) {
		ERROR_LOG("eRROR");
		return false;
	}

	DumpState(
		dummy, 
		true
	);

	m_FinishInit = true;

	return true;
}
*/

/*
bool ProcessState::DumpState(
	_In_ StateFiles& File,
	_In_ bool _ExitAccessProcessState) {
	
	bool res = true;

	do {
		if (m_FinishInit) {
			res = EnterAccessProcessState();
			if (!res) {
				ERROR_LOG("Failed to enter access process state");
				break;
			}
		}
		

		auto targetProcess =
			TargetProcess::GetInstance().GetProcess();
		auto targetId =
			TargetProcess::GetInstance().GetProcessId();

		res = MiniDumpWriteDump(
			targetProcess,
			targetId,
			File.DmpFile.GetHandle(),
			MINI_DUMP_FLAGS,
			nullptr,
			nullptr,
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to generate a mini dump");
			break;
		}
	} while (false);


	if (_ExitAccessProcessState) {
		res = ExitAccessProcessState();
		if (!res) {
			ERROR_LOG("Failed to exit access process state");
		}
	}

	return res;
}
*/

bool ProcessState::Init(_In_ std::shared_ptr<RAIIDirectory> TargetDir) {
	m_TargetDir = TargetDir;
	return true;
}

bool ProcessState::DumpState(_In_ bool _ExitAccessProcessState) {
	bool res = true;

	do {
		res = EnterAccessProcessState();
		if (!res) {
			ERROR_LOG("Failed to enter access process state");
			break;
		}

		res = m_DmpFile.Dump(m_TargetDir);
		if (!res) {
			ERROR_LOG("Failed to dump state");
			break;
		}

		if (_ExitAccessProcessState) {

			res = ExitAccessProcessState();
			if (!res) {
				ERROR_LOG("Failed to exit access process state");
				break;
			}
		}
	} while (false);

	return res;
}

bool ProcessState::RevertState(
	_In_ ProcessState& CurrentState,
	_In_ bool _EnterAccessProcessState) {

	bool res = true;

	do {
		if (_EnterAccessProcessState) {
			res = EnterAccessProcessState();
			if (!res) {
				ERROR_LOG("Failed to enter access process state");
				break;
			}
		}

		StateInfo currentState;
		StateInfo targetState;

		res = CurrentState.m_DmpFile.Read(currentState);
		if (!res) {
			ERROR_LOG("Failed to read current state");
			break;
		}
		res = m_DmpFile.Read(targetState);
		if (!res) {
			ERROR_LOG("Failed to read state");
			break;
		}
		
		res = ActualRevertState(
			currentState, 
			targetState
		);
		if (!res) {
			ERROR_LOG("Failed to recover state");
			break;
		}

		res = ExitAccessProcessState();
		if (!res) {
			ERROR_LOG("Failed to exit access process state");
			break;
		}
	} while (false);

	return res;
}

bool ProcessState::ActualRevertState(
	_In_ StateInfo& CurrentState,
	_In_ StateInfo& TargetState) {

	bool res = true;

	do {
		res = RecoverThreadsState(
			CurrentState, 
			TargetState
		);
		if (!res) {
			ERROR_LOG("Failed to recover threads state");
			break;
		}
	} while (false);

	return res;
}

bool ProcessState::RecoverThreadsState(
	_In_ StateInfo& CurrentState,
	_In_ StateInfo& TargetState) {
	
	bool res = true;

	do {
		ThreadsState currentThreadsState;
		ThreadsState targetThreadsState;

		res = currentThreadsState.Init(CurrentState);
		if (!res) {
			ERROR_LOG("Failed to init current threads state");
			break;
		}
		res = targetThreadsState.Init(TargetState);
		if (!res) {
			ERROR_LOG("Failed to init target threads state");
			break;
		}

		res = targetThreadsState.Revert(currentThreadsState);
		if (!res) {
			ERROR_LOG("Failed to revert threads state");
			break;
		}

	} while (false);

	return res;
}


bool ProcessState::ExitAccessProcessState() {
	bool res = true;


	do {
		res = TargetProcess::GetInstance().Resume();
		if (!res) {
			ERROR_LOG("Failed to resume target process");
			break;
		}
	} while (false);

	return res;
}

bool ProcessState::EnterAccessProcessState() {
	bool res = true;

	do {


		res = TargetProcess::GetInstance().Suspend();
		if (!res) {
			ERROR_LOG("Failed to suspend target process");
			break;
		}

		/*
		if (!m_URSDll.IsLoaded()) {

			res = m_URSDll.Load();
			if (!res) {
				ERROR_LOG("Failed to load dll");
				break;
			}
		}
		else {
			m_URSDll.ResumeDllThread();
		}
		*/

	} while (false);

	return res;
}

/*
bool ProcessState::RevertState(
	_In_ StateFiles& File,
	_In_ StateFiles& CurrentState,
	_In_ bool _EnterAccessProcessState) {

	auto& dmpFile = File.DmpFile;
	auto& currentDmpFile = CurrentState.DmpFile;

	bool res = true;

	do {
		if (_EnterAccessProcessState) {
			res = EnterAccessProcessState();
			if (!res) {
				ERROR_LOG("Failed to enter access process state");
				break;
			}
		}
		
		RAIIFileMapping dmpMapping(
			dmpFile, 
			dmpFile.GetRAIIFileSize()
		);
		RAIIFileMapping currentDmpMapping(
			currentDmpFile, 
			dmpFile.GetRAIIFileSize()
		);


		StateInfo info;
		LPVOID mappedAddr = dmpMapping.GetMappedAddr();
		if (mappedAddr == nullptr) {
			res = false;
			break;
		}

		res = ReadDumpFileState(
			mappedAddr,
			info
		);
		info.FilePath = dmpFile.GetPath();

		if (!res) {
			ERROR_LOG(FAILED_TO_READ_DUMP_FILE);
			break;
		}

		mappedAddr = dmpMapping.GetMappedAddr();
		if (mappedAddr == nullptr) {
			res = false;
			break;
		}
		StateInfo currentStateInfo;
		res = ReadDumpFileState(
			currentDmpMapping.GetMappedAddr(),
			currentStateInfo
		);
		if (!res) {
			ERROR_LOG(FAILED_TO_READ_DUMP_FILE);
			break;
		}

		res = RevertToStateFromInfo(
			info,
			currentStateInfo
		);
		if (!res) {
			ERROR_LOG(FAILED_TO_UNMAP_FILE_FROM_MEMORY);
			break;
		}


	} while (false);

	res = ExitAccessProcessState();
	if (!res) {
		ERROR_LOG("Failed to exit access process state");
	}

	return res;
}
*/
/*
bool ProcessState::ReadDumpFileState(
	_In_ LPVOID Addr,
	_Out_ StateInfo& Info) {

	bool res = true;

	do {
		PMINIDUMP_DIRECTORY dir;
		ULONG size;

		Info.DumpFileBaseAddr = Addr;

		res = MiniDumpReadDumpStream(
			Addr,
			ThreadListStream,
			&dir,
			(LPVOID*)&Info.ThreadInfo,
			&size
		);
		if (!res) {
			ERROR_LOG("Failed to read minidump threads state");
			break;
		}
		res = MiniDumpReadDumpStream(
			Addr,
			MemoryInfoListStream,
			&dir,
			(LPVOID*)&Info.MemInfo,
			&size
		);
		if (!res) {
			ERROR_LOG("Failed to read minidump memory info state");
			break;
		}
		res = MiniDumpReadDumpStream(
			Addr,
			Memory64ListStream,
			&dir,
			(LPVOID*)&Info.Mem,
			&size
		);
		if (!res) {
			ERROR_LOG("Failed to read minidump memory state");
			break;
		}
		res = MiniDumpReadDumpStream(
			Addr,
			HandleDataStream,
			&dir,
			(LPVOID*)&Info.HandlesInfo,
			&size
		);
		if (!res) {
			ERROR_LOG("Failed to read minidump handles state");
			break;
		}
		res = MiniDumpReadDumpStream(
			Addr,
			ModuleListStream,
			&dir,
			(LPVOID*)&Info.ModulesInfo,
			&size
		);
		if (!res) {
			ERROR_LOG("Failed to read minidump modules state");
			break;
		}

	} while (false);

	return res;
}
*/

/*
bool ProcessState::RevertToStateFromInfo(
	_In_ StateInfo& Info,
	_In_ StateInfo& CurrentStateInfo) {

	bool res = true;

	do {
		res = RevertThreadsState(
			Info,
			CurrentStateInfo
		);
		if (!res) {
			ERROR_LOG("Failed to revert threads state");
			break;
		}
		res = RevertMemoryState(
			Info,
			CurrentStateInfo
		);
		if (!res) {
			ERROR_LOG("Failed to revert memory state");
			break;
		}
		res = TargetProcess::GetInstance().Resume();
		if (!res) {
			ERROR_LOG("Failed to resume target process");
			break;
		}
	} while (false);

	return res;
}

bool ProcessState::RevertThreadsState(
	_In_ StateInfo& TargetState,
	_In_ StateInfo& CurrentState) {

	bool res = true;

	auto currentThreadsInfo = CurrentState.ThreadInfo;
	auto threadsInfo = TargetState.ThreadInfo;

	do {
		if (!res) {
			ERROR_LOG("Failed to get target threads list");
			break;
		}

		if (currentThreadsInfo->NumberOfThreads != threadsInfo->NumberOfThreads) {
			ERROR_LOG(THREADS_MISS_MATCH_ERROR);
			break;
		}

		int cRecoveredThreads = 0;
		for (ULONG32 i = 0; i < threadsInfo->NumberOfThreads; i++) {
			auto thread = threadsInfo->Threads[i];

			for (ULONG32 j = 0; j < currentThreadsInfo->NumberOfThreads; j++) {
				auto currentStateThread = currentThreadsInfo->Threads[j];

				if (currentStateThread.ThreadId != thread.ThreadId) {
					continue;
				}

				auto raiiThread = RAIIThread();
				res = raiiThread.Open(currentStateThread.ThreadId);
				if (!res) {
					ERROR_LOG("Failed to open thread");
					break;
				}

				auto threadContextBaseAddr =
					(ULONG64)TargetState.DumpFileBaseAddr + thread.ThreadContext.Rva;
				CONTEXT context;
				memcpy(
					&context,
					(void*)threadContextBaseAddr,
					sizeof(CONTEXT)
				);
				context.ContextFlags = CONTEXT_ALL;
				res = raiiThread.SetContext(context);
				if (!res) {
					ERROR_LOG("Failed to set thread context");
					break;
				}

				cRecoveredThreads++;
			}
		}
		if (cRecoveredThreads != threadsInfo->NumberOfThreads) {
			ERROR_LOG(THREADS_MISS_MATCH_ERROR);
			break;
		}

	} while (false);

	return res;
}

bool ProcessState::ResolveAllocations() {
	bool res = true;

	do {

	} while (false);

	return res;
}

bool ProcessState::RevertMemoryState(
	_In_ StateInfo& TargetState,
	_In_ StateInfo& CurrentState) {

	bool res = true;

	MemInfoDict oldMemInfo;
	MemInfoDict newMemInfo;
	MemInfoDict allBuffers;

	auto process =
		TargetProcess::GetInstance().GetProcess();

	do {

		/*
		* resolveAllocations: currentAllocs targetAllocs
		* for range in currentAllocs
		*	allocs = targetAllocs.findInRange(range, out equals)
		*	
		*	if equals
		*		continue
		* 
		*	free range
		*	
		*	for alloc allocs
		*		allocMem alloc
		* 
		*/


		/*
		*
		*	resolveAllocations()
		* 
		*	if newBuffer found oldBuffer
		*		if newBuffer.type != oldBuffer.type
		*			error not supported same mem addr realoc
		*			continue
		*		if newBuffer.size != oldBuffer.size
		*			realocBlocks.push(oldBuffer)
		*
		*		newBuffers.remove(newBuffer)
		*		oldBuffers.remove(newBuffer)
		*
		*	for buffer in newBuffers
		*		free buffer
		*	for buffer in oldBuffers
		*		alloc buffer
		*	for bufffer in realoc
		*		realoc buffer
		*
		*	for buffer in allBuffers
		*		copy mem old buffer to new buffer
		*		set old prot
		*
		*
		*
		*
		*/
	
/*
		res = ResolveAllocations();
		if (!res) {
			ERROR_LOG("Failed to resove allocations");
			break;
		}

		MakeMemInfoDict(
			TargetState,
			oldMemInfo
		);
		MakeMemInfoDict(
			CurrentState,
			newMemInfo
		);

		/*
		allBuffers = oldMemInfo;

		std::list<ULONG64> remove;

		for (auto buffer : oldMemInfo) {

			auto oldBuffer = newMemInfo.find(buffer.first);

			if (oldBuffer == newMemInfo.end()) {
				continue;
			}

			if (buffer.second->Type != oldBuffer->second->Type) {
				EXIT_WITH_ERROR("Not supported non matching mem types on same addr");
				break;
			}

			if (buffer.second->RegionSize != oldBuffer->second->RegionSize) {
				continue;
			}

			remove.push_back(oldBuffer->first);
		}
		for (auto addr : remove) {
			oldMemInfo.erase(addr);
			newMemInfo.erase(addr);
		}

		for (auto buffer : newMemInfo) {
			if (buffer.second->State == MEM_FREE) {
				continue;
			}
			if (buffer.second->Type == MEM_MAPPED) {
				ERROR_LOG("Dealoc");
				ERROR_LOG(NON_PRIVATE_NEM_NOT_SUPPORTED);
				WARNING_LOG(MemToString(buffer.second));
				continue;
			}
			if (buffer.second->Type == MEM_IMAGE) {
			}
			else {

				struct Range {
					UINT64 StartAddr;
					UINT64 EndAddr;

				};
				static std::list<Range> mem;



				res = VirtualFreeEx(
					process,
					(LPVOID)buffer.second->BaseAddress,
					0,
					MEM_RELEASE
				);


				if (!res && GetLastError() == 487) {
					for (auto range : mem) {
						if (range.StartAddr >= buffer.second->BaseAddress && buffer.second->BaseAddress <= range.EndAddr) {
							ERROR_LOG("aAAAA");
						}
					}

					MEMORY_BASIC_INFORMATION i;
					VirtualQueryEx(process, (LPVOID)buffer.second->BaseAddress, &i, sizeof(i));

					WARNING_LOG(MemToString(buffer.second));
					ERROR_LOG("Failed to free virtual mem");
					break;
				}
				mem.push_back(Range{ buffer.second->BaseAddress, buffer.second->BaseAddress + buffer.second->RegionSize });
			}
		}

		for (auto buffer : oldMemInfo) {
			if (buffer.second->State == MEM_FREE) {
				continue;
			}

			if (buffer.second->Type == MEM_MAPPED) {
				ERROR_LOG("Alloc");
				ERROR_LOG(NON_PRIVATE_NEM_NOT_SUPPORTED);
				WARNING_LOG(MemToString(buffer.second));

				continue;
			}

			auto size = CalcRegionSize(
				buffer.second->RegionSize, 
				buffer.second->AllocationBase, 
				buffer.second->BaseAddress
			);

			
			if (buffer.second->Type == MEM_IMAGE) {
				
			}
			
			else {

				res = VirtualAllocEx(
					process,
					(LPVOID)buffer.second->BaseAddress,
					size,
					buffer.second->State,
					buffer.second->Protect
				);
				if (!res) {
					WARNING_LOG(MemToString(buffer.second));
					ERROR_LOG("Failed to allocate virtual mem");
					break;
				}
			}
		}

		for (auto buffer : allBuffers) {
			auto size = CalcRegionSize(
				buffer.second->RegionSize,
				buffer.second->AllocationBase,
				buffer.second->BaseAddress
			);

			if (buffer.second->State == MEM_FREE || buffer.second->State == MEM_RESERVE) {
				continue;
			}
			if (buffer.second->Protect == PAGE_EXECUTE_READ) {
				continue;
			}
			if (buffer.second->Type == MEM_IMAGE) {
				continue;
			}

			ULONG oldProt;
			res = VirtualProtectEx(
				process,
				(LPVOID)buffer.second->BaseAddress,
				size,
				PAGE_READWRITE,
				&oldProt
			);
			if (!res) {
				if (buffer.second->Type == MEM_MAPPED && buffer.second->Protect == PAGE_READONLY) {
					continue;
				}

				WARNING_LOG("Failed to protect mem at address: " << buffer.second->AllocationBase);
				WARNING_LOG(MemToString(buffer.second));
				continue;
			}
			UINT64 addr;
			res = FindStartAddrOfMemBlock(
				TargetState,
				buffer.second->BaseAddress,
				addr
			);
			if (res) {
				res = WriteProcessMemory(
					process,
					(LPVOID)buffer.second->BaseAddress,
					(LPVOID)addr,
					size,
					nullptr
				);

				if (!res) {
					ERROR_LOG("Failed to write process memory");
					WARNING_LOG(MemToString(buffer.second));
				}

			}

			res = VirtualProtectEx(
				process,
				(LPVOID)buffer.second->BaseAddress,
				size,
				buffer.second->Protect,
				&oldProt
			);
			if (!res) {
				ERROR_LOG("Failed to reprotect mem at address: " << buffer.second->AllocationBase);
				continue;
			}
		}


		res = true;
		*/
/*
	} while (false);
	

	return res;
}




void ProcessState::MakeMemInfoDict(
	_In_ StateInfo& State,
	_In_ MemInfoDict& TargetMemInfo) {


	auto current = (PMINIDUMP_MEMORY_INFO)(&State.MemInfo[1]);
	for (int i = 0; i < State.MemInfo->NumberOfEntries; i++) {


		if (TargetMemInfo.find(current->BaseAddress) == TargetMemInfo.end())
			TargetMemInfo.insert(MemInfoPair(current->BaseAddress, current));
		
		current++;
	}
}



bool ProcessState::FindStartAddrOfMemBlock(
	_In_ StateInfo& TargetState,
	_In_ ULONG64 TargetAddr,
	_Out_ ULONG64& Addr) {


	auto mem = TargetState.Mem;
	Addr = mem->BaseRva + (UINT64)TargetState.DumpFileBaseAddr;

	for (int i = 0; i < mem->NumberOfMemoryRanges; i++) {
		auto desc = mem->MemoryRanges[i];
		auto upperAddr = desc.StartOfMemoryRange + desc.DataSize;

		if (desc.StartOfMemoryRange == TargetAddr) {

			return true;
		}

		Addr += desc.DataSize;

	}

	return false;

}



bool ProcessState::GetModuleNameFromAddr(
	_In_ StateInfo& TargetState,
	_In_ UINT64 Addr,
	_Out_ std::string& Name) {

	auto info = TargetState.ModulesInfo;
	auto current = info->Modules;
	for (int i = 0; i < info->NumberOfModules; i++) {
		if (current->BaseOfImage == Addr) {
			auto str = (PMINIDUMP_STRING)(
				(UINT64)TargetState.DumpFileBaseAddr +
				current->ModuleNameRva
				);

			using convert_typeX = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_typeX, wchar_t> converterX;

			Name = converterX.to_bytes(str->Buffer);
			return true;
		}

		current++;
	}

	return false;
}

UINT64 ProcessState::CalcRegionSize(
	_In_ UINT64 RegionSize,
	_In_ UINT64 AllocBase,
	_In_ UINT64 Base) {

	//return (Base - AllocBase) + RegionSize;
	return RegionSize;
}
*/