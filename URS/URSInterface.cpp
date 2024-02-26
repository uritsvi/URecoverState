#include "include\URSInterface.h"
#include "URS.h"
#include "..\Common\include\Common.h"

URS g_URS;

bool InitURS(
	_In_ DWORD PID,
	_In_ DebuggerFunctions Functions) {

	return g_URS.Init(
		PID,
		Functions
	);
}

bool URSCaptureState() {
	return g_URS.CaptureState();

}
bool URSRecoverState() {
	return g_URS.RecoverState();

}

bool URSCompareSnapshots(_Out_ bool& Equals) {
	return g_URS.CompareSnapshots(Equals);
}