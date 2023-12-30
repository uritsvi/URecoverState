#include "ThreadsState.h"
#include "Error.h"
#include "RAII/include/RAIIThread.h"

typedef std::pair<DWORD, PCONTEXT> ThreadPair;


ThreadsState::ThreadsState() {
}
ThreadsState::~ThreadsState() {
}

bool ThreadsState::Init(StateInfo& State) {
	bool res = true;
	auto threadsInfo = State.ThreadInfo;

	m_NumOfThreads = threadsInfo->NumberOfThreads;

	do {
		for (int i = 0; i < m_NumOfThreads; i++) {
			auto thread = threadsInfo->Threads[i];

			PCONTEXT context = 
				(PCONTEXT)((UINT64)State.Mapping.GetMappedAddr() + 
					thread.ThreadContext.Rva);

			m_Threads.insert(ThreadPair(
				thread.ThreadId, 
				context)
			);
		}
	} while (false);

	return res;
}
bool ThreadsState::Revert(ThreadsState& Current) {
	bool res = true;
	
	if (m_Threads.size() != Current.m_Threads.size()) {
		ERROR_LOG("Threads miss match");
		return false;
	}

	for (auto& thread : Current.m_Threads) {
		
		auto targetThread = 
			m_Threads.find(thread.first);
		
		if (targetThread == m_Threads.end()) {
			ERROR_LOG("Threads miss match");
			break;
		}

		auto raiiThread = RAIIThread();
		res = raiiThread.Open(targetThread->first);
		if (!res) {
			ERROR_LOG("Failed to open thread");
			break;
		}

		CONTEXT context;
		memcpy(
			&context,
			(void*)targetThread->second,
			sizeof(CONTEXT)
		);
		context.ContextFlags = CONTEXT_ALL;
		res = raiiThread.SetContext(&context);
		if (!res) {
			ERROR_LOG("Failed to set thread context");
			break;
		}
	}

	return res;
}