#pragma once

#include <Windows.h>
#include <tlhelp32.h>

#include <list>
#include <string>

class TargetProcess
{
public:
	static TargetProcess& GetInstance();

	bool Init(_In_ DWORD Pid);
	
	HANDLE GetProcess();
	DWORD GetProcessId();

	//bool GetTargetThreadsList(_Out_ std::list<THREADENTRY32>& Threads);
	//bool FreeAllProcessPages();

	bool LoadDllToTargetProcess(
		_In_ std::string DllPath, 
		_Out_ DWORD& Tid
	);

	bool Suspend();
	bool Resume();


private:
	HANDLE m_Process;
	DWORD m_Pid;
};

