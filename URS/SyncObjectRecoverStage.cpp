#include "SyncObjectRecoverStage.h"
#include "..\Common\include\Error.h"

struct EventInfo {

};
bool MakeEventInfo(_Inout_ HandleEntry& Entry) {

}

bool SyncObjectRecoverStage::Init(_In_ SnapshotData& SnapshotData) {


	return true;
}

bool SyncObjectRecoverStage::CaptureObjectState(
	_Inout_ HandleEntry& Entry,
	_Out_ bool& ContinueSearch) {
	
	bool res = true;

	do {
		ContinueSearch = false;

		switch (Entry.HandleEntry.ObjectType)
		{
		case PSS_OBJECT_TYPE_SEMAPHORE: {

		}break;
		case PSS_OBJECT_TYPE_MUTANT: {

		}break;
		case PSS_OBJECT_TYPE_EVENT: {
			res = MakeEventInfo(Entry);
			if (!res) {
				ERROR_LOG("Failed to make event info");
				break;
			}
		}break;
		default:
			ContinueSearch = true;
			break;
		}
		if (ContinueSearch) {
			break;
		}

		Entry.ObjectInfo.DataType = OBJECT_INFO_SYNC_OBJECT;

	} while (false);

	return res;
}

bool SyncObjectRecoverStage::MakeHandle(
	_In_ HandleEntry& Entry,
	_Out_ bool& ContinueSearch,
	_Out_ std::shared_ptr<RAIIHandle>& NewHandle) {

	return true;
}