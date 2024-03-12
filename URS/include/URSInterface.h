#pragma once

#include <Windows.h>
#include "../Common/include/Common.h"


bool InitURS(
	_In_ DWORD PID,
	_In_ DebuggerFunctions Functions
);


bool URSPreProcessInit();
bool URSPostProcessInit();

bool URSCaptureState();
bool URSRecoverState();
bool URSCompareSnapshots(_Out_ bool& Equals);