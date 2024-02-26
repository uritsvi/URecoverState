#pragma once

#include <Windows.h>
#include "..\RAII\include\RAIIProcess.h"

#include <memory>

class Breakpoint
{
public:
	Breakpoint();
	~Breakpoint();

	bool Create(
		_In_ std::shared_ptr<RAIIProcess> TargetProcess,
		_In_ LPVOID Addr
	);

	bool Enable();
	bool Disable();

private:
	CHAR m_BpData;

	LPVOID m_Addr;
	std::shared_ptr<RAIIProcess> m_TargetProcess;
};

