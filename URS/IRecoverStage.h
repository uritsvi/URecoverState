#pragma once

#include <Windows.h>

#include <ProcessSnapshot.h>

#include <unordered_map>
#include <memory>

#include "..\RAII\include\RAIIHandle.h"

typedef std::pair<HANDLE, PSS_HANDLE_ENTRY> HandleDictEntry;
typedef std::pair<DWORD, PSS_THREAD_ENTRY> ThreadsDictEntry;

typedef std::unordered_map<HANDLE, PSS_HANDLE_ENTRY> HandlesDict;
typedef std::unordered_map<DWORD, PSS_THREAD_ENTRY> ThreadsDict;

struct SnapshotData {
	std::shared_ptr<HandlesDict> HandleEntries;
	std::shared_ptr<ThreadsDict> Threads;
};

struct HandleEntry {
	HANDLE HandleVal;
	LPVOID ObjectAddr;
	PSS_HANDLE_ENTRY HandleEntry;
};

class IRecoverStage
{
public:
	virtual bool Init(_In_ SnapshotData& SnapshotData) = 0;

	virtual bool MakeHandle(
		_In_ HandleEntry& Entry, 
		_Out_ bool& ContinueSearch, 
		_Out_ std::shared_ptr<RAIIHandle>& NewHandle
	) = 0;
};

