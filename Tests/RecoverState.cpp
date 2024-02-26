#include <Windows.h>

#include "RecoverState.h"
#include "../Common/include/Common.h"

#include "../Common/include/Error.h"

RecoverState& RecoverState::GetInstance() {
	static RecoverState instance;
	return instance;
}

bool RecoverState::Init() {
	bool res = true;


	do {
		m_SharedMem = (SharedMem*)VirtualAlloc(
			(LPVOID)TESTS_SHARED_MEM, 
			TESTS_OP_FLAG_SIZE_IN_BYTES,
			MEM_COMMIT | MEM_RESERVE, 
			PAGE_READWRITE
		);
		if (m_SharedMem == nullptr) {
			res = false;
			ERROR_LOG("Failed to allocate virtal memory");
			break;
		}
	} while (false);

	return res;
}

bool RecoverState::SetState(
	_In_ UINT Op,
	_In_ std::string StateName) {
	
	m_SharedMem->Op = Op;
	m_SharedMem->StateStrLen = StateName.length() + 1;
	
	strcpy_s(
		(char*)&m_SharedMem->StateStr,
		m_SharedMem->StateStrLen,
		StateName.c_str()
	);


	return true;
}

bool RecoverState::HandleOp(_In_ UINT Op, _In_ std::string StateName) {
	bool res = true;

	do {
		res = SetState(
			Op,
			StateName
		);
		if (!res) {
			ERROR_LOG("Failed to set state");
			break;
		}
	} while (false);

	return res;
}

bool RecoverState::SetCaptureInfo(_In_ std::string StateName) {
	return HandleOp(
		TESTS_OP_CAPTURE, 
		StateName
	);

}
bool RecoverState::SetRecoverInfo(_In_ std::string StateName) {
	return HandleOp(
		TESTS_OP_REVERT,
		StateName
	);
}