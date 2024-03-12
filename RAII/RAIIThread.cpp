#include <iostream>

#include "include\RAIIThread.h"
#include "include\RAIIProcess.h"
#include "include\RAIIHandle.h"
#include "..\NTDLL\include\NTDLL.h"

#include "processthreadsapi.h"

typedef struct _THREAD_BASIC_INFORMATION {




	NTSTATUS                ExitStatus;
	PVOID                   TebBaseAddress;
	CLIENT_ID               ClientId;
	KAFFINITY               AffinityMask;
	KPRIORITY               Priority;
	KPRIORITY               BasePriority;



} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

RAIIThread::RAIIThread() {
	m_Handle = nullptr;
}
RAIIThread::~RAIIThread() {
	std::cout << "Closeeeeeeeeeeee: " << std::to_string(m_Tid) << "\n";

}

void RAIIThread::SetThreadDesc() {
	SetThreadDescription(m_Handle->GetHandle(), L"RAIIThread");
	
}

bool RAIIThread::Open(DWORD Tid) {
	bool res = true;

	do {

		HANDLE handle = OpenThread(
			THREAD_ALL_ACCESS,
			FALSE,
			Tid
		);
		if (handle == INVALID_HANDLE_VALUE) {
			res = false;
			break;
		}

		m_Tid = Tid;
		m_Handle = std::make_shared<RAIIHandle>(handle);

		std::cout << "Opennnnn: " << std::to_string(Tid) << "\n";

		SetThreadDesc();

	} while (false);


	return res;
	
}

bool RAIIThread::Create(
	_In_ std::shared_ptr<RAIIProcess> Process,
	_In_ LPVOID Addr,
	_In_ LPVOID Ctx,
	_In_ DWORD Flags,
	_Out_ DWORD& Tid) {

	bool res = true;

	do {
		HANDLE handle = 
			CreateRemoteThread(
				Process->GetRAIIHandle()->GetHandle(),
				nullptr, 
				0, 
				(LPTHREAD_START_ROUTINE)Addr, 
				Ctx, 
				Flags, 
				&Tid
			);
		if (handle == nullptr) {
			res = false;
			break;
		}
		m_Tid = Tid;
		m_Handle = std::make_shared<RAIIHandle>(handle);

		SetThreadDesc();

	} while (false);

	return res;
}

bool RAIIThread::Create(
	_In_ LPTHREAD_START_ROUTINE Function,
	_In_ LPVOID Ctx,
	_Out_ DWORD& Tid) {
	bool res = true;

	do {
		HANDLE handle = CreateThread(
			nullptr, 
			0, 
			Function, 
			Ctx, 
			0, 
			&Tid
		);
		if (handle == nullptr) {
			res = false;
			break;
		}

		m_Tid = Tid;
		m_Handle = std::make_shared<RAIIHandle>(handle);

		SetThreadDesc();

	} while (false);

	return res;
}


bool RAIIThread::GetContext(_In_ PCONTEXT Context) {
	return GetThreadContext(
		m_Handle->GetHandle(),
		Context
	);
}
bool RAIIThread::SetContext(_In_ PCONTEXT Context) {
	bool res = true;
	
	do {
		res = SetThreadContext(
			m_Handle->GetHandle(),
			Context
		);
		if (!res) {
			break;
		}
	} while (false);

	return res;
}

bool RAIIThread::Suspend() {
	if (m_Handle == nullptr) {
		
		;
	}

	DWORD res = SuspendThread(m_Handle->GetHandle());
	if (res == -1) {
		return false;
	}

	return true;
}

bool RAIIThread::Resume() {
	if (m_Handle == nullptr) {
		return false;
	}

	DWORD res = ResumeThread(m_Handle->GetHandle());
	if (res == -1) {
		return false;
	}

	return true;
}

bool RAIIThread::Terminate() {
	TerminateThread(
		m_Handle->GetHandle(), 
		0
	);
	return true;
}

bool RAIIThread::WaitForTerminate(_Out_ DWORD& Code, DWORD TimeOutInSeconds) {
	
	bool res = true;
	ULONG64 numOfCicles = TimeOutInSeconds * 2;


	int c = 0;
	do {
		if (c == numOfCicles) {
			res = false;
			break;
		}
		c++;

		Sleep(500);
		res = GetExitCodeThread(
			m_Handle->GetHandle(),
			&Code
		);
	} while (res && Code == STILL_ACTIVE);
	
	return res;
}

std::shared_ptr<RAIIHandle> RAIIThread::GetRAIIHandle() {
	return m_Handle;
}

DWORD RAIIThread::GetTid() {
	return m_Tid;
}


bool RAIIThread::FourceResume() {

	int suspendCount;
	do {
		suspendCount = ResumeThread(m_Handle->GetHandle());
	} while (suspendCount != 0);

	return true;
}

bool RAIIThread::GetThreadSuspendCount(_Out_ DWORD& C) {
	C = SuspendThread(m_Handle->GetHandle());
	ResumeThread(m_Handle->GetHandle());
	return true;
}
bool RAIIThread::SetSuspendCount(_In_ DWORD C) {
	int current = SuspendThread(m_Handle->GetHandle());
	if (current == C) {
		return true;
	}
	
	if(current < C){

		for (int i = 0; i < C - current; i++) {
			SuspendThread(m_Handle->GetHandle());
		}
		return true;
	}

	for (int i = 0; i < current - C; i++) {
		ResumeThread(m_Handle->GetHandle());
	}
	return true;
}

LPVOID RAIIThread::GetTEB() {
	

		THREAD_BASIC_INFORMATION info;
		NTSTATUS status = NTDLL::GetInstance().NtQueryInformationThread(
			m_Handle->GetHandle(),
			(THREADINFOCLASS)0,
			&info,
			sizeof(info),
			0
		);
		if (status != CMC_STATUS_SUCCESS) {
			return nullptr;
		}

		return info.TebBaseAddress;


}
