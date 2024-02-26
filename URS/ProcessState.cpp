#include <string>

#include "ProcessState.h"
#include "..\Common\include\TargetProcess.h"

#include "..\RAII\include\RAIIHandle.h"
#include "..\RAII\include\RAIIBuffer.h"

#include "..\Common\include\Error.h"

#include "..\NTDLL\include\NTDLL.h"

#define PSS_CAPTURE_FLAG (PSS_CAPTURE_HANDLES | \
						  PSS_CAPTURE_HANDLE_NAME_INFORMATION | \
					      PSS_CAPTURE_HANDLE_BASIC_INFORMATION | \
					      PSS_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION | \
						  PSS_CAPTURE_HANDLE_TRACE | \
						  PSS_CAPTURE_THREADS | \
						  PSS_CAPTURE_THREAD_CONTEXT | \
						  PSS_CAPTURE_THREAD_CONTEXT_EXTENDED) 

bool ProcessState::CapturePSS() {
	bool res = true;

	do {
		HPSS snapshot;
		res = PssCaptureSnapshot(
			TargetProcess::GetInstance().GetProcess()->GetRAIIHandle()->GetHandle(),
			PSS_CAPTURE_FLAG,
			CONTEXT_ALL,
			&snapshot
		);
		if (res != ERROR_SUCCESS) {
			res = false;
			ERROR_LOG("Failed to capture snapshot");
			break;
		}

		m_Snapshot.Create(snapshot);
	} while (false);

	return (res == ERROR_SUCCESS);
}

bool ProcessState::CaptureHandleInfo() {
	bool res = true;

	do {
		auto targetProcess = 
			TargetProcess::GetInstance().GetProcess();

		RAIIBuffer dummyBuffer;
		int dummyBufferSize = 32; // I dont know why

		dummyBuffer.Create(dummyBufferSize);
		ULONG size = 0;
		NTDLL::GetInstance().NtQuerySystemInformation(
			(PROCESSINFOCLASS)SystemHandleInformation,
			dummyBuffer.GetPtr(),
			dummyBufferSize,  
			&size
		);

		RAIIBuffer buffer;
		res =buffer.Create(size);
		if (!res) {
			ERROR_LOG("Failed to create buffer");
			break;
		}
		NTDLL::GetInstance().NtQuerySystemInformation(
			(PROCESSINFOCLASS)SystemHandleInformation,
			buffer.GetPtr(),
			size,
			nullptr
		);

		auto systemHandles = 
			(PSYSTEM_HANDLE_INFORMATION)buffer.GetPtr();
	
		for (ULONG i = 0; i < systemHandles->NumberOfHandles; i++) {
			auto currentEntry = 
				systemHandles->Handles[i];


			if (currentEntry.UniqueProcessId == targetProcess->GetPID()) {
				HandleEntry entry;
				
				entry.HandleVal = (HANDLE)currentEntry.HandleValue;
				entry.ObjectAddr = currentEntry.Object;

				m_CapturedHandles.push_back(entry);
			}
		}

	} while (false);

	return res;
}

bool ProcessState::ExtructSnapshotData(
	_In_ Walkers& Walkers,
	_Out_ SnapshotData& Info) {

	bool res = true;

	do {
		Info.HandleEntries = std::make_shared<HandlesDict>();
		Info.Threads = std::make_shared<ThreadsDict>();

		PSS_HANDLE_ENTRY handleEntry;
		PSS_THREAD_ENTRY threadEntry;

		do {
			res = PssWalkSnapshot(
				m_Snapshot.GetHPSS(),
				PSS_WALK_HANDLES,
				Walkers.HandlesWalk.GetWALK(),
				&handleEntry,
				sizeof(handleEntry)
			);
			if (res != ERROR_SUCCESS) {
				break;
			}

			Info.HandleEntries->insert(HandleDictEntry(
				handleEntry.Handle,
				handleEntry
			));

		} while (true);
		do {
			res = PssWalkSnapshot(
				m_Snapshot.GetHPSS(),
				PSS_WALK_THREADS,
				Walkers.ThreadWalk.GetWALK(),
				&threadEntry,
				sizeof(threadEntry)
			);
			if (res != ERROR_SUCCESS) {
				break;
			}

			Info.Threads->insert(ThreadsDictEntry(
				threadEntry.ThreadId,
				threadEntry
			));

		} while (true);


		res = true;

	} while (false);

	return res;
}

bool ProcessState::Capture() {
	bool res = true;

	do {
		res = CapturePSS();
		if (!res) {
			ERROR_LOG("Failed to capture pss snapshot");
			break;
		}
		res = CaptureHandleInfo();
		if (!res) {
			ERROR_LOG("Failed to capture handles info");
			break;
		}
	} while (false);

	return res;
}

void ProcessState::RegisterRecoverStage(_In_ std::shared_ptr<IRecoverStage> Stage) {
	m_RecoverStages.push_back(Stage);
}

bool ProcessState::RecoverState() {
	bool res = true;

	do {
		Walkers walkers;

		SnapshotData snapshotData;
		res = ExtructSnapshotData(
			walkers, 
			snapshotData
		);
		if (!res) {
			ERROR_LOG("Failed to extruct snapshot info");
			break;
		}

		for (auto recoverStage : m_RecoverStages) {
			res = recoverStage->Init(snapshotData);
			if (!res) {
				ERROR_LOG("Failed to load data");
				break;
			}
		}
		if (!res) {
			ERROR_LOG("Failed to load data");
			break;
		}
		for (auto handle : m_CapturedHandles) {
			for (auto recoverStage : m_RecoverStages) {

				bool continueSearch;
				std::shared_ptr<RAIIHandle> raiiHandle;

				continueSearch = true;
				auto handleInfo = snapshotData.HandleEntries->find(handle.HandleVal)->second;
				handle.HandleEntry = handleInfo;
				res = recoverStage->MakeHandle(
					handle, 
					continueSearch, 
					raiiHandle
				);
				if (!res) {
					auto handleEntry = 
						snapshotData.HandleEntries->find(handle.HandleVal);

					ERROR_LOG(
						std::string("Failed to recover handle index: ") + 
						std::to_string((ULONG)handle.HandleVal)
					);
				}

				if (!continueSearch) {
					break;
				}
			}
		}

	} while (false);

	return res;
}