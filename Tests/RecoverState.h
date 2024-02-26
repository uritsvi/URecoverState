#pragma once

#include <Windows.h>
#include <string>
#include "../Common/include/Common.h"




class RecoverState
{
public:
	static RecoverState& GetInstance();

	bool Init();

	bool SetCaptureInfo(_In_ std::string StateName);
	bool SetRecoverInfo(_In_ std::string StateName);

private:
	bool SetState(
		_In_ UINT Op,
		_In_ std::string StateName
	);
	bool HandleOp(_In_ UINT Op, _In_ std::string StateName);

private:
	SharedMem* m_SharedMem;
};

#define CAPTURE( STATE_NAME ) RecoverState::GetInstance().SetCaptureInfo(STATE_NAME); __debugbreak();
#define REVERT( STATE_NAME ) RecoverState::GetInstance().SetRecoverInfo(STATE_NAME); __debugbreak();