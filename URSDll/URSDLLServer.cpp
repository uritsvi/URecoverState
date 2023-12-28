#include "URSDLLServer.h"
#include "..\Common.h"

#include "..\Error.h"
#include "..\RAII\include\RAIIFile.h"

DWORD DllRun(_In_ LPVOID Ctx) {
	auto instance = reinterpret_cast<URSDLLServer*>(Ctx);
	return instance->SelfDllRun();
}

bool URSDLLServer::Init() {
	bool res = true;
	do {
		for (int i = 0; i < m_FunctionsTable.size(); i++) {
			m_FunctionsTable[i] = 0;
		}

		DWORD runThreadTid;
		res = m_RunThread.Create(
			DllRun, 
			this, 
			runThreadTid
		);
		if (!res) {
			ERROR_LOG("Failed to create thread to run dll");
			break;
		}

		res = InitSharedMemory(runThreadTid);
		if (!res) {
			ERROR_LOG("Failed to init shared memory");
			break;
		}

		res = RegisterDllFunctions();
		if (!res) {
			ERROR_LOG("Failled to init dll functions");
			break;
		}
	} while (false);

	return res;
}

bool URSDLLServer::InitSharedMemory(_In_ DWORD ThreadTId) {
	bool res = true;

	do {
		m_SharedMemory = std::make_shared<SharedMemory>();
		res = m_SharedMemory->Open();
		if (!res) {
			ERROR_LOG("Failed to init shared memory");
			break;
		}

		res = m_DllFunctions.Init(m_SharedMemory);
		if (!res) {
			ERROR_LOG("Failed to init dll functions");
			break;
		}

		DWORD clientTid;
		bool res = m_SharedMemory->Read(
			0,
			sizeof(clientTid),
			(char*)&clientTid
		);
		if (!res) {
			ERROR_LOG("Failed to read from shread memory");
			break;
		}
		m_SharedMemory->SetTargetTid(clientTid);

		res = ReturnOp(
			res,
			ThreadTId
		);
		if (!res) {
			ERROR_LOG("Failed to send msg to client");
		}
	} while (false);

	return res;
}


DWORD URSDLLServer::SelfDllRun() {
	DWORD res = 0;
	
	UINT msg;
	WPARAM Wparam;
	LPARAM LParam;

	while (true) {
		std::cout << "1\n";


		res = m_SharedMemory->ReciveMsg(
			msg, 
			Wparam, 
			LParam
		);

		std::cout << "2\n";


		if (!res) {
			ERROR_LOG("Failed to recive msg");
			break;
		}

		if (!res) {
			ERROR_LOG("Failed to recive msg");
			break;
		}



		switch (msg) {
			case FUNCTION_CALL_OP:
				HandleFunctionCallOp(
					Wparam, 
					LParam
				); break;
			default:
				ERROR_LOG("Unknone op"); break;
		}
	}

	return res;
}

bool URSDLLServer::RegisterDllFunction(
	_In_ UINT Index,
	_In_ DllFunction Function) {

	if (m_FunctionsTable[Index] != 0) {
		ERROR_LOG("Failed to register dll function");
		return false;
	}

	m_FunctionsTable[Index] = Function;

	return true;
}

bool URSDLLServer::ReturnOp(
	_In_ bool Res, 
	_In_ LPARAM LParam) {

	UINT finishCode = ERROR_OP;
	if (Res) {
		finishCode = SUCCESS_OP;
	}

	return m_SharedMemory->SendMsg(
		FINISH_OP,
		finishCode,
		LParam
	);
}

bool URSDLLServer::HandleFunctionCallOp(
	_In_ WPARAM WParam,
	_In_ LPARAM LParam) {

	bool res = false;
	UINT64 returnVal = 0;

	do {
		if (m_FunctionsTable[WParam] == nullptr) {
			break;
		}

		returnVal = 
			m_FunctionsTable[WParam](
				&m_DllFunctions,
				LParam
			);

	} while (false);

	return ReturnOp(
		res, 
		returnVal
	);

	return true;

}

bool URSDLLServer::RegisterDllFunctions() {
	bool res = true;

	do {
		
		res = RegisterDllFunction(
			MAP_FILE_FUNCTION, 
			UMapViewOfFile
		);
		if (!res) {
			ERROR_LOG("Failed to register dll function");
			break;
		}
		res = RegisterDllFunction(
			UNMAP_FILE_FUNCTION,
			UUnMapViewOfFile
		);
		if (!res) {
			ERROR_LOG("Failed to register dll function");
			break;
		}
		
	} while (false);

	return res;

}
