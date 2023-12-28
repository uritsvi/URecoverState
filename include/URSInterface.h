#pragma once

#include <Windows.h>

#define EXPORT __declspec(dllexport)

EXPORT bool InitURS(_In_ DWORD PID);

EXPORT bool URSTakeSnapshot();
EXPORT bool URSRevertToSnapshot();
