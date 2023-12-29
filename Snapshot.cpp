#include "Snapshot.h"
#include "TargetProcess.h"
#include "Error.h"
#include "ProcessState.h"

int Snapshot::m_SnapshotCount;

/*
std::string Snapshot::MakeSnapshotFilePath() {
	return std::string("Snapshot") + std::to_string(m_Index);
}
*/

Snapshot::Snapshot() {
	m_Index = m_SnapshotCount++;
	//m_TargetFilePath = MakeSnapshotFilePath();
}
Snapshot::Snapshot(std::string Path) {
	m_SnapshotCount++;
	//m_TargetFilePath = Path;
}

Snapshot::~Snapshot() {
}

bool Snapshot::InternalTakeSnapshot(bool ExitAccessProcessState) {
	bool res = true;

	do {
		if (!res) {
			ERROR_LOG("Failed to get file name");
		}

		/*
		res = m_TargetFile.Create(m_TargetFilePath, true);
		if (!res) {

		}
		
		
		res = ProcessState::GetInstance().DumpState(
			m_TargetFile, 
			ExitAccessProcessState
		);
		if (!res) {
			ERROR_LOG("Failed to write to target file");
			break;
		}
		*/
		
		/*
		* TODO: Move this
		*/
		m_ProcessStateDir = 
			std::make_shared<RAIIDirectory>();
		
		res = m_ProcessStateDir->Create(
			std::to_string(m_Index), 
			true
		);
		if (!res) {
			ERROR_LOG("Failed to create dir");
			break;
		}

		res = m_ProcessState.Init(m_ProcessStateDir);
		if (!res) {
			ERROR_LOG("Failed to init process state");
			break;
		}

		res = m_ProcessState.DumpState(
			ExitAccessProcessState
		);
		if (!res) {
			ERROR_LOG("Failed to dump process state");
			break;
		}

	} while (false);

	return res;
}

bool Snapshot::Take() {
	
	
	bool res =  InternalTakeSnapshot(true);

	return res;
}
bool Snapshot::Revert(_Out_ std::shared_ptr<Snapshot>& CurrentState) {
	TargetProcess::GetInstance().Suspend();

	bool res = true;

	do {
		CurrentState = std::make_shared<Snapshot>();
		

		/*
		if (!m_TargetFile.IsOpen()) {
			m_TargetFile.Open(m_TargetFilePath);
		}
		*/


		res = CurrentState->InternalTakeSnapshot(false);
		if (!res) {
			ERROR_LOG("Failed to take snapshot");
			break;
		}
		res = m_ProcessState.RevertState(
			CurrentState->m_ProcessState,
			false
		);
		if (!res) {
			ERROR_LOG("Failed to revert state");
			break;
		}

		/*
		if (!m_TargetFile.IsOpen()) {
			m_TargetFile.Open(m_TargetFilePath);
		}

		res = ProcessState::GetInstance().RevertState(
			m_TargetFile, 
			CurrentState->m_TargetFile,
			false
		);
		if (!res) {
			ERROR_LOG("Failed to revert process state");
			break;
		}
		*/

	} while (false);

	//TargetProcess::GetInstance().Resume();


	return res;
}
