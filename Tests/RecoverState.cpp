#include <Windows.h>

#include "RecoverState.h"
#include "../Common.h"

RecoverState& RecoverState::GetInstance() {
	static RecoverState instance;
	return instance;
}

bool RecoverState::Capture() {
	ULONG_PTR arg;
	arg = UNIT_TEST_CAPTURE;

	RaiseException(
		UNIT_TEST_EXCEPION_CODE, 
		0, 
		1, 
		&arg
	);
	return true;
}
bool RecoverState::Recover() {
	ULONG_PTR arg;
	arg = UNIT_TEST_REVERT;

	RaiseException(
		UNIT_TEST_EXCEPION_CODE,
		0,
		1,
		&arg
	);
	return true;
}