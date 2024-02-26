#include "include/SharedMemory.h"
#include "../Common/include/Common.h"

#include <iostream>

bool SharedMemory::Create() {
	bool res = true;

	do {
		HANDLE fileMapping = CreateFileMappingA(
			INVALID_HANDLE_VALUE,
			nullptr,
			PAGE_READWRITE,
			0,
			SHARED_REGION_SIZE,
			SHARED_MEMORY_REGION_NAME
		);
		if (fileMapping == nullptr) {
			res = false;
			break;
		}

		m_SharedBufferAddr = MapViewOfFile(
			fileMapping,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			SHARED_REGION_SIZE
		);
		if (m_SharedBufferAddr == nullptr) {
			res = false;
			break;
		}

		memset(
			m_SharedBufferAddr, 
			0, 
			SHARED_REGION_SIZE
		);

	} while (false);

	return res;
}

void SharedMemory::SetTargetTid(_In_ DWORD PID) {
	m_TargetClientPID = PID;
}


bool SharedMemory::Open() {
	bool res = true;
	do {

		HANDLE fileMapping = CreateFileMappingA(
			INVALID_HANDLE_VALUE,
			nullptr,
			PAGE_READWRITE,
			0,
			SHARED_REGION_SIZE,
			SHARED_MEMORY_REGION_NAME
		);
		if (fileMapping == nullptr) {
			res = false;
			break;
		}

		m_SharedBufferAddr = MapViewOfFile(
			fileMapping,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			SHARED_REGION_SIZE
		);
		if (m_SharedBufferAddr == nullptr) {
			res = false;
			break;
		
		}

	} while (false);

	return res;
}

bool SharedMemory::Read(
	_In_ ULONG64 Offset,
	_In_ ULONG64 Size,
	_Out_ char* Buffer) {

	auto addr = (UINT64)m_SharedBufferAddr + Offset;

	memcpy(
		Buffer,
		(char*)addr,
		Size
	);

	return true;

}
bool SharedMemory::Write(
	_In_ ULONG64 Offset,
	_In_ ULONG64 Size,
	_In_ char* Buffer) {


	auto addr = (UINT64)m_SharedBufferAddr + Offset;

	memcpy(
		(char*)addr,
		Buffer,
		Size
	);

	return true;
}

bool SharedMemory::ReadStr(
	_In_ ULONG64 MaxSize,
	_In_ ULONG64 Offset,
	_Out_ char* Buffer,
	_Out_ ULONG64& Len) {

	auto addr = (UINT64)m_SharedBufferAddr + Offset;
	Len = strlen((const char*)m_SharedBufferAddr);

	//memcpy(Buffer, (char*)addr, Len);
	strcpy(
		(char*)Buffer,
		(char*)addr
	);

	return true;
}
bool SharedMemory::WriteStr(
	_In_ const char* Buffer,
	_In_ ULONG64 Offset) {
	
	auto addr = (UINT64)m_SharedBufferAddr + Offset;

	strcpy(
		(char*)addr,
		Buffer
	);

	return true;
}

bool SharedMemory::SendMsg(
	_In_ UINT Msg,
	_In_ WPARAM WParam,
	_In_ LPARAM LParam) {

	bool res = true;

	do {

		res = PostThreadMessageA(
			m_TargetClientPID,
			Msg,
			WParam,
			LParam
		);

		
	} while (false);

	return res;
}
bool SharedMemory::ReciveMsg(
	_Out_ UINT& Msg,
	_Out_ WPARAM& WParam,
	_Out_ LPARAM& LParam) {

	bool res = true;

	do {
		MSG msg;
		res = GetMessageA(
			&msg,
			nullptr,
			0,
			0
		);
		if (!res) {
			break;
		}

		Msg = msg.message;
		WParam = msg.wParam;
		LParam = msg.lParam;

	} while (false);

	return res;
}

bool SharedMemory::ReciveMsg(_Out_ UINT& Msg) {

	WPARAM wparam = 0;
	LPARAM lparam = 0;

	return ReciveMsg(
		Msg,
		wparam,
		lparam
	);
}