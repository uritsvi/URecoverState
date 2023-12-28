#include "Snapshot.h"
#include "TargetProcess.h"
#include "Error.h"
#include "ProcessState.h"

int Snapshot::m_SnapshotCount;

std::string Snapshot::MakeSnapshotFilePath() {
	return std::string("Snapshot") + std::to_string(m_Index);
}

Snapshot::Snapshot() {
	m_Index = m_SnapshotCount++;
	m_TargetFilePath = MakeSnapshotFilePath();
}
Snapshot::Snapshot(std::string Path) {
	m_SnapshotCount++;
	m_TargetFilePath = Path;
}

Snapshot::~Snapshot() {
}

bool Snapshot::InternalTakeSnapshot(bool ExitAccessProcessState) {
	bool res = true;

	do {
		if (!res) {
			ERROR_LOG("Failed to get file name");
		}
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
	} while (false);

	return res;
}

std::string MemToString(MEMORY_BASIC_INFORMATION& Info) {
	std::string out = "";
	out += "AllocationBase: " + std::to_string((UINT64)Info.BaseAddress) + "\n";
	out += "AllocationBase: " + std::to_string((UINT64)Info.AllocationBase) + "\n";
	out += "AllocationProtect: " + std::to_string(Info.AllocationProtect) + "\n";
	out += "RegionSize: " + std::to_string(Info.RegionSize) + "\n";
	out += "State: " + std::to_string(Info.State) + "\n";
	out += "Protect: " + std::to_string(Info.Protect) + "\n";
	out += "Type: " + std::to_string(Info.Type) + "\n";

	return out;
}


bool Snapshot::Take() {
	

	TargetProcess::GetInstance().Suspend();
	


	bool res =  InternalTakeSnapshot(true);

	TargetProcess::GetInstance().Resume();


	return res;
}
bool Snapshot::Revert(_Out_ std::shared_ptr<Snapshot>& CurrentState) {
	TargetProcess::GetInstance().Suspend();

	bool res = true;

	do {
		CurrentState = std::make_shared<Snapshot>();
		
		std::string str;

		LPCVOID addr = 0;
		MEMORY_BASIC_INFORMATION info;


		if (!m_TargetFile.IsOpen()) {
			m_TargetFile.Open(m_TargetFilePath);
		}


		res = CurrentState->InternalTakeSnapshot(false);
		if (!res) {
			ERROR_LOG("Failed to take snapshot");
			break;
		}

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

	} while (false);

	TargetProcess::GetInstance().Resume();


	return res;
}
