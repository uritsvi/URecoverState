#pragma once

#include "IRecoverStage.h"

class SyncObjectRecoverStage : public IRecoverStage
{
public:
	bool Init(_In_ SnapshotData& SnapshotData);

	bool CaptureObjectState(
		_Inout_ HandleEntry& Entry,
		_Out_ bool& ContinueSearch
	);

	bool MakeHandle(
		_In_ HandleEntry& Entry,
		_Out_ bool& ContinueSearch,
		_Out_ std::shared_ptr<RAIIHandle>& NewHandle
	);
};

