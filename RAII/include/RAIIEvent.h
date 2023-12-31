#pragma once

#include <Windows.h>

class RAIIEvent {

public:
	RAIIEvent();
	~RAIIEvent();

	bool Create();

	bool Set();
	bool Wait();
	bool Reset();

private:
	HANDLE m_Handle;
};