#include "ThreadsRecoverStage.h"

#include "..\Common\include\TargetProcess.h"
#include "..\Common\include\Error.h"

#include "..\URSDriver\include\URSDriverInterface.h"

bool ThreadsRecoverStage::Init(_In_ SnapshotData& SnapshotData) {
	
	bool res = true;

	DWORD tid;
	for (auto& thread : *SnapshotData.Threads) {
		if (thread.second.ExitStatus != STILL_ACTIVE) {
			continue;
		}

		auto newThread = std::make_shared<RAIIThread>();
		res = newThread->Create(
			TargetProcess::GetInstance().GetProcess(), 
			0, 
			0, 
			CREATE_SUSPENDED, 
			tid
		);
		if (!res) {
			ERROR_LOG(
				"Failed to create thread, Target tid: " + 
				std::to_string(thread.first)
			);
			break;
		}

		res = newThread->SetContext(thread.second.ContextRecord);
		if (!res) {
			ERROR_LOG("Failed to set new thread context");
			break;
		}

		auto targetTid = thread.first;
		res = URSDriverMoveThreadId(
			targetTid, 
			tid
		);
		if (!res) {
			ERROR_LOG("Failed to move thread id");
			break;
		}

		res = URSDriverChangeThreadTEB(
			targetTid, 
			thread.second.TebBaseAddress
		);
		if (!res) {
			ERROR_LOG("Failed to change thread teb address");
			break;
		}

		m_Threads.insert(NewThreadDictPair(
			targetTid, 
			newThread
		));
	}

	return res;
}

bool ThreadsRecoverStage::MakeHandle(
	_In_ HandleEntry& Entry,
	_Out_ bool& ContinueSearch,
	_Out_ std::shared_ptr<RAIIHandle>& NewHandle) {

	bool res = true;

	do {
		if (Entry.HandleEntry.ObjectType != PSS_OBJECT_TYPE_THREAD) {
			break;
		}

		auto targetTid = 
			Entry.HandleEntry.TypeSpecificInformation.Thread.ThreadId;
		auto thread = m_Threads.find(targetTid);
		
		if (thread == m_Threads.end()) {
			ERROR_LOG(
				"Failed to find new thread with id: " + 
				std::to_string(targetTid)
			);
			
			res = false;
			break;
		}


		NewHandle = 
			thread->second->GetRAIIHandle();

		ContinueSearch = false;
	} while (false);

	return res;
}