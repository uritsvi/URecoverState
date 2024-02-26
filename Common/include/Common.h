#pragma once


#define OP_LOAD_SYMBOLS 0x800

#define OP_GET_KERNEL_BASE 0x801

#define OP_MOVE_THREAD_ID 0x802

#define OP_CHANGE_THREAD_TEB 0x803

#define OP_CHANGE_PEB 0x804

#define OP_CHANGE_HANDLE 0x805


#define IOCTL_DEVICE 0x8022

#define IOCTL_LOAD_SYMBOLS CTL_CODE( \
				IOCTL_DEVICE, OP_LOAD_SYMBOLS, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_GET_KERNEL_BASE CTL_CODE( \
				IOCTL_DEVICE, OP_GET_KERNEL_BASE, METHOD_NEITHER, FILE_ANY_ACCESS)
				
#define IOCTL_CHANGE_HANDLE CTL_CODE( \
				IOCTL_DEVICE, OP_CHANGE_HANDLE, METHOD_NEITHER, FILE_ANY_ACCESS)

/*
* The src thread should be suspendent befaure entering this op
*/
#define IOCTL_MOVE_THREAD_ID CTL_CODE( \
				IOCTL_DEVICE, OP_MOVE_THREAD_ID , METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_CHANGE_PROCESS_ID CTL_CODE( \
				IOCTL_DEVICE, OP_CHANGE_PEB , METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_CHANGE_THREAD_TEB CTL_CODE( \
				IOCTL_DEVICE, OP_CHANGE_THREAD_TEB , METHOD_NEITHER, FILE_ANY_ACCESS)

#pragma warning(disable : 4200)


#ifdef _KERNEL_MODE
	#include "../KURSDriver/Def.h"
#else
	typedef PVOID64 pExpLookupHandleTableEntry;
	typedef PVOID64 pExDestroyHandle;
	typedef PVOID64 pExCreateHandle;
	typedef PVOID64 pExLockHandleTableEntry;
	typedef PVOID64 pExUnlockHandleTableEntry;
#endif

struct KernelInfo {
	PVOID64 BaseAddr;
	DWORD64 Size;
};
struct KernelFunctions{
	PVOID64 pPspCidTable;
	pExpLookupHandleTableEntry ExpLookupHandleTableEntry;
	pExDestroyHandle ExDestroyHandle;
	pExCreateHandle ExCreateHandle;
	pExLockHandleTableEntry ExLockHandleTableEntry;
	pExUnlockHandleTableEntry ExUnlockHandleTableEntry;

	SIZE_T NumOfKernelFunction;
};
struct MoveThreadIdParams {
	HANDLE TargetId;
	HANDLE SrcId;
};
struct ChangeThreadTEBParams {
	HANDLE ThreadId;
	PVOID64 NewTEB;
};
struct ChangeProcessIdParams {
	HANDLE ProcessId;
	HANDLE TargetId;
};
struct ChangeHandleParams {
	HANDLE SrcHandle;
	HANDLE TargetHandle;
	HANDLE Pid;
};

struct SharedMem {
	UINT32 Op;
	UINT32 StateStrLen;
	CHAR StateStr[0];
};

#ifndef _KERNEL_MODE

#include <memory>

class RAIIProcess;

typedef bool(*DetachDebuggerFunction)();
typedef bool(*ReatachDebuggerFunction)(_In_ std::shared_ptr<RAIIProcess> NewTarget);

struct DebuggerFunctions {
	DetachDebuggerFunction DetachDebugger;
	ReatachDebuggerFunction ReatachDebugger;
};

#endif

typedef bool(*UContinueThread)(ULONG Tid);
typedef bool(*UContinueAll)();



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

#define UCLONE_TARGET_PROCESS 0x407

#define SUCCESS_OP 1
#define ERROR_OP 0

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

#define MAX_OBJS 0x100
	