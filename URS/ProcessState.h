#pragma once

#include <memory>

#include "..\RAII\include\RAIIProcess.h"
#include "IRecoverStage.h"

#include <list>
#include <vector>

#include "ProcessSnapshot.h"

class RAIIHPSS {

public:
	void Create(HPSS HPSS) {
		m_HPSS = HPSS;
	}
	~RAIIHPSS() {
		PssFreeSnapshot(
			GetCurrentProcess(), 
			m_HPSS
		);
	}


	HPSS GetHPSS() {
		return m_HPSS;
	}

private:
	HPSS m_HPSS;
};

class RAIIHPSSWALK {

public:
	void Create() {
		PssWalkMarkerCreate(
			NULL, 
			&m_Walk
		);
	}
	~RAIIHPSSWALK() {
		PssWalkMarkerFree(
			m_Walk
		);
	}

	HPSSWALK GetWALK() {
		return m_Walk;
	}

private:
	HPSSWALK m_Walk;
};


struct Walkers {

public:
	Walkers() {
		ThreadWalk.Create();
		HandlesWalk.Create();
	}

	RAIIHPSSWALK ThreadWalk;
	RAIIHPSSWALK HandlesWalk;
};

class ProcessState
{
public:
	bool Capture();
	void RegisterRecoverStage(_In_ std::shared_ptr<IRecoverStage> Stage);
	bool RecoverState();

private:
	bool CapturePSS();
	bool CaptureHandleInfo();

	bool ExtructSnapshotData(
		_In_ Walkers& Walkers,
		_Out_ SnapshotData& Info
	);

private:
	std::list<std::shared_ptr<IRecoverStage>> m_RecoverStages;
	
	std::vector<HandleEntry> m_CapturedHandles;
	RAIIHPSS m_Snapshot;
};

