#pragma once

#include "RAII/include/RAIIFile.h"

#include <minidumpapiset.h>
#include <tlhelp32.h>

#include <vector>
#include <unordered_map>

#include "URSDLL.h"
#include "RAII/include/RAIIDirectory.h"
#include "MemAllocInfo.h"
#include "DmpFile.h"
#include "ThreadsState.h"
#include "MemoryState.h"

/*
struct StateFiles {
	RAIIFile DmpFile;
//	RAIIFile AllocCSV;

	bool Create(std::string Name, bool DeleteOnClose=false) {
		bool res = DmpFile.Create(Name + ".dmp",DeleteOnClose);
		if (!res) { 
			return res; 
		}
		//res = AllocCSV.Create(Name + ".csv", DeleteOnClose);
		
		m_isOpen = true;
		
		return res;
	}
	bool Open(std::string Name) {
		bool res = DmpFile.Open(Name + ".dmp");
		if (!res) {
			return res;
		}
		//res = AllocCSV.Open(Name + ".csv");
		
		m_isOpen = true;
		
		return res;
	}

	bool IsOpen() {
		return m_isOpen;
	}

private:
	bool m_isOpen = false;
};
*/

class ProcessState
{
public:
	//static ProcessState& GetInstance();

	bool Init(_In_ std::shared_ptr<RAIIDirectory> TargetDir);

	/*
	* When we want to revert to a snapshot we dont want the process to resume 
	* execution between the taking of the snapshot and the actual revert. 
	* So we can chose if the function will resume the process 
	*/

	/*
	* Both functions recives open file handles
	*/
	bool DumpState(
		_In_ bool _ExitAccessProcessState
	);
	bool RevertState(
		_In_ ProcessState& CurrentState,
		_In_ bool _EnterAccessProcessState
	);

	/*
	bool RevertState(
		_In_ StateFiles& File,
		_In_ StateFiles& CurrentState,
		_In_ bool _EnterAccessProcessState
	);
	*/


private:

	bool ActualRevertState(
		_In_ StateInfo& CurrentState,
		_In_ StateInfo& TargetState
	);

	bool RecoverThreadsState(
		_In_ StateInfo& CurrentState,
		_In_ StateInfo& TargetState);
	bool RecoverMemoryState(
		_In_ StateInfo& CurrentState,
		_In_ StateInfo& TargetState);

	bool EnterAccessProcessState();
	bool ExitAccessProcessState();

	/*
	bool ReadDumpFileState(
		_In_ LPVOID Addr,
		_Out_ StateInfo& Info
	);
	*/

	/*
	bool RevertToStateFromInfo(
		_In_ StateInfo& Info,
		_In_ StateInfo& CurrentStateInfo
	);

	bool RevertThreadsState(
		_In_ StateInfo& TargetState,
		_In_ StateInfo& CurrentState
	);
	bool RevertMemoryState(
		_In_ StateInfo& TargetState,
		_In_ StateInfo& CurrentState
	);



	bool EnterAccessProcessState();
	bool ExitAccessProcessState();

	bool FindStartAddrOfMemBlock(
		_In_ StateInfo& TargetState,
		_In_ ULONG64 TargetAddr, 
		_Out_ ULONG64& Addr
	);

	bool GetModuleNameFromAddr(
		_In_ StateInfo& TargetState,
		_In_ UINT64 Addr,
		_Out_ std::string& Name
	);

	bool ResolveAllocations();

	void MakeMemInfoDict(
		_In_ StateInfo& State,
		_In_ MemInfoDict& targetMemInfo
	);

	UINT64 CalcRegionSize(
		_In_ UINT64 RegionSize, 
		_In_ UINT64 AllocBase, 
		_In_ UINT64 Base
	);
	*/

private:


	// Deletes the dir affter deleted all files in dir
	std::shared_ptr<RAIIDirectory> m_TargetDir;
	
	// Calles this destructor first 
	DmpFile m_DmpFile; 
	
};

