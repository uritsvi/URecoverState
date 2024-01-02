#pragma once

#include <Windows.h>
#include <string>
#include "..\Common.h"


class RecoverState
{
public:
	static RecoverState& GetInstance();

	bool Init();

	bool Capture(_In_ std::string StateName);
	bool Recover(_In_ std::string StateName);

private:
	bool SetState(
		_In_ UINT Op,
		_In_ std::string StateName
	);
	bool HandleOp(_In_ UINT Op, _In_ std::string StateName);

private:
	SharedMem* m_SharedMem;
};

