#pragma once

#include <Windows.h>

#include <vector>
#include <memory>

#include "..\Common\include\Common.h"
#include "..\RAII\include\RAIIProcess.h"

#include "ProcessState.h"

#include "URSDll.h"

class URS
{
public:
	bool Init(
		_In_ DWORD PID,
		_In_ DebuggerFunctions Functions);


	bool CaptureState();
	bool RecoverState();
	bool CompareSnapshots(_Out_ bool& Equals);

private:
	bool RecoverMappedMemPages(_In_ std::shared_ptr<RAIIProcess> TargetProcess);

private:
	DebuggerFunctions m_Functions;
	std::shared_ptr<RAIIProcess> m_CloneOfProcess;

	URSDLL m_URSDLL;

	ProcessState m_ProcessState;
};

