#pragma once

struct SharedMem {
	UINT Op;
	UINT StateStrLen;
	CHAR StateStr[0];
};

/*
* Params: WPARAM = PID 
*/
#define INIT_URSDLL 0x400 


/*
* Params: WPRAM = Finish code, LPARAM = Return value
*/
#define FINISH_OP 0x401


/*
* Params: WPARAM = FunctionIndexInTable
* All function parameters are passed at offset 0 from shared memory start
*/
#define FUNCTION_CALL_OP 0x402

/*
* Params: LPARAM = Map address 
* Shared memory = file path
*/
#define MAP_FILE_FUNCTION 0x403

/*
* Params: LPARAM = Map address 
*/
#define UNMAP_FILE_FUNCTION 0x404

#define SUCCESS_OP 0 
#define ERROR_OP 1

#define MAX_DLL_FUNCTIONS (1 << 16)
#define SHARED_REGION_SIZE (1 << 16)

#define SHARED_MEMORY_REGION_NAME "URSDLL"

/*
* A shared address that the tests use to determain 
* The op type (Capture / Revert).
* 
* This address is ignored whene recovering memory state
* In Debug mode 
* 
*/
#ifdef _DEBUG
	#define TESTS_SHARED_MEM 0x69690000
	#define TESTS_OP_FLAG_SIZE_IN_BYTES 4096

	#define TESTS_OP_CAPTURE 0
	#define TESTS_OP_REVERT 1
#endif

struct HeapAllocParams {
	HANDLE HeapHandle;
	DWORD  dwFlags;
	SIZE_T dwBytes;
};
struct HeapFreeParams {
	HANDLE HeapHandle;
	LPVOID Addr;
};

