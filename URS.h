#pragma once

#include <Windows.h>

#include "Snapshot.h"
#include <vector>
#include <memory>

class URS
{
public:
	bool Init(_In_ DWORD PID);


	bool TakeSnapshot();
	bool RevertToSnapshot();
	
private:
	bool LoadAllSnapshots();

private:
	std::vector<std::shared_ptr<Snapshot>> m_Snapshots;
};

