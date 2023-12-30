#pragma once

#include "DmpFile.h"

#include <unordered_map>

class ThreadsState
{
public:
	ThreadsState();
	~ThreadsState();

	bool Init(StateInfo& State);

	bool Revert(ThreadsState& Current);

private:
	std::unordered_map<DWORD, PCONTEXT> m_Threads;
	UINT m_NumOfThreads;
};

