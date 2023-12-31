#pragma once

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

#define UNIT_TEST_EXCEPION_CODE 0x6969
#define UNIT_TEST_CAPTURE 0x420
#define UNIT_TEST_REVERT 0x69

struct HeapAllocParams {
	HANDLE HeapHandle;
	DWORD  dwFlags;
	SIZE_T dwBytes;
};
struct HeapFreeParams {
	HANDLE HeapHandle;
	LPVOID Addr;
};

