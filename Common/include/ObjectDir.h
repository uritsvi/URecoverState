#pragma once

#include <string>
#include <memory>

#include <Windows.h>


class RAIIHandle;

class ObjectDir {

public:
	bool Create(_In_ std::wstring Name);

	HANDLE GetHandle();

private:
	HANDLE m_Handle;
};