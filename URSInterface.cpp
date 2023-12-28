#include "include\URSInterface.h"
#include "URS.h"

URS g_URS;

bool InitURS(_In_ DWORD PID) {
	return g_URS.Init(PID);
}

bool URSTakeSnapshot() {
	return g_URS.TakeSnapshot();

}
bool URSRevertToSnapshot() {
	return g_URS.RevertToSnapshot();

}
