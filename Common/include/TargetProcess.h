#pragma once


#include <Windows.h>
#include <tlhelp32.h>

#include <list>
#include <string>
#include <memory>

class RAIIProcess;

class TargetProcess
{
public:
	static TargetProcess& GetInstance();

	bool Init(_In_ DWORD Pid);
	

	std::shared_ptr<RAIIProcess> GetProcess();
	void SetProcess(_In_ std::shared_ptr<RAIIProcess> Process);


private:
	std::shared_ptr<RAIIProcess> m_Process;
	DWORD m_Pid;
};

