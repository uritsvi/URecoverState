#include "URS.h"
#include "Error.h"
#include "TargetProcess.h"
#include "ProcessState.h"

#include "RAII/include/RAIIDirectory.h"
#include "NTDLL.h"

#pragma comment(lib, "RAII.lib")


bool URS::Init(_In_ DWORD PID) {
	bool res = true;
	
	do {
		res = NTDLL::GetInstance().Init();
		if (!res) {
			ERROR_LOG("Failed to init ntdll");
			break;
		}

		res = TargetProcess::GetInstance().Init(PID);
		if (!res) {
			ERROR_LOG("Failed to init target process");
			break;
		}
		res = LoadAllSnapshots();
		if (!res) {
			ERROR_LOG("Failed to load snapshots");
			break;
		}
	} while (false);

	return res;
}

bool URS::LoadAllSnapshots() {
	/*
	bool res = true;
	
	do {

		char path[MAX_PATH];

		res = GetCurrentDirectoryA(
			MAX_PATH,
			path
		);
		if (!res) {
			ERROR_LOG("Failed to get current directory");
			break;
		}

		RAIIDirectory dir;
		res = dir.Open(path);
		if (!res) {
			ERROR_LOG("Failed to open dir");
			break;
		}

		std::list<std::string> snapshotFiles;
		res = dir.FilesWithExtension(
			".dmp", 
			snapshotFiles
		);
		dir.FilesWithExtension(
			".csv",
			snapshotFiles
		);
		if (!res) {
			ERROR_LOG("Failed to get files with extension");
			break;
		}

		for (auto path : snapshotFiles) {
			res = DeleteFileA(path.c_str());
			if (!res) {
				ERROR_LOG("Failed to delete dump file");
				break;
			}
		}
	} while (false);

	return res;
	*/

	return true;
}

bool URS::TakeSnapshot() {
	bool res = true;

	do {
		auto snapshot = std::make_shared<Snapshot>();
		res = snapshot->Take();
		if (!res) {
			ERROR_LOG("Failed to take snapshot");
			break;
		}

		m_Snapshots.push_back(snapshot);
	} while (false);

	return res;
}
bool URS::RevertToSnapshot() {
	bool res = true;

	do {

		std::shared_ptr<Snapshot> current;

		auto snapshot = m_Snapshots[0];
		res = snapshot->Revert(current);
		if (!res) {
			ERROR_LOG("Failed to revert to snapshot");
			break;
		}


	} while (false);

	return res;
}