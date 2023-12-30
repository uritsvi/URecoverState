#pragma once

#include <memory>

#include "RAII/include/RAIIFile.h"
#include "ProcessState.h"

class Snapshot
{
public:
	Snapshot();
	~Snapshot();

	Snapshot(std::string Path);

	bool Take();

	/*
	* Recives an uninitialized shared ptr to a snapshot
	*/
	bool Revert(_Out_ std::shared_ptr<Snapshot>& CurrentState);

private:
	//std::string MakeSnapshotFilePath();

	bool InternalTakeSnapshot(bool ExitAccessProcessState);

private:
	ProcessState m_ProcessState;

	std::shared_ptr<RAIIDirectory> m_ProcessStateDir;
	//std::string m_TargetFilePath;
	//StateFiles m_TargetFile;
	int m_Index;
	static int m_SnapshotCount;

};

