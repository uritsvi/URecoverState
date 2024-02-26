#include "Breakpoint.h"
#include "..\Common\include\Error.h"
#include "..\RAII\include\RAIIHandle.h"

#define BP_INST 0xcc

Breakpoint::Breakpoint() {
}
Breakpoint::~Breakpoint() {
}

bool Breakpoint::Create(
	_In_ std::shared_ptr<RAIIProcess> TargetProcess,
	_In_ LPVOID Addr) {

	m_TargetProcess = TargetProcess;
	m_Addr = Addr;

	return true;
}


bool Breakpoint::Enable() {
	bool res = true;

	do {
		res = m_TargetProcess->ReadMem(
			m_Addr, 
			&m_BpData, 
			sizeof(m_BpData)
		);
		if (!res) {
			ERROR_LOG("Failed to read process mem");
			break;
		}

		char bp = BP_INST;
		res = m_TargetProcess->WriteMem(
			m_Addr, 
			&bp, 
			sizeof(bp)
		);
		if (!res) {
			ERROR_LOG("Failed to write to process mem");
			break;
		}
		res = FlushInstructionCache(
			m_TargetProcess->GetRAIIHandle()->GetHandle(),
			m_Addr,
			1
		);
		if (!res) {
			ERROR_LOG("Failed to flush instruction cach");
			break;
		}
	} while (false);

	return res;
}
bool Breakpoint::Disable() {
	bool res = true;

	do {
		res = m_TargetProcess->WriteMem(
			m_Addr,
			&m_BpData,
			sizeof(m_BpData)
		);
		if (!res) {
			ERROR_LOG("Failed to write to process mem");
			break;
		}

		res = FlushInstructionCache(
			m_TargetProcess->GetRAIIHandle()->GetHandle(),
			m_Addr, 
			1
		);
		if (!res) {
			ERROR_LOG("Failed to flush instruction cach");
			break;
		}

	} while (false);

	return res;
}
