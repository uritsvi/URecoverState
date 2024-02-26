#pragma once

#include "IRecoverStage.h"
#include "..\RAII\include\RAIIThread.h"

typedef std::pair<DWORD, std::shared_ptr<RAIIThread>> NewThreadDictPair;

class ThreadsRecoverStage : public IRecoverStage
{
public:
	bool Init(_In_ SnapshotData& SnapshotData);

	bool MakeHandle(
		_In_ HandleEntry& Entry,
		_Out_ bool& ContinueSearch,
		_Out_ std::shared_ptr<RAIIHandle>& NewHandle
	);

private:
	std::unordered_map<DWORD, std::shared_ptr<RAIIThread>> m_Threads;
};

