#pragma once

#define SystemHandleInformation 0X10


/*
* Structs are from https://www.unknowncheats.me/forum/3387112-post1.html
*/


typedef struct _HANDLE_TABLE_ENTRY_INFO
{
	ULONG AuditMask;                //Uint4B
	ULONG MaxRelativeAccessMask;    //Uint4b
} HANDLE_TABLE_ENTRY_INFO, * PHANDLE_TABLE_ENTRY_INFO;

struct A {
	ULONG64 Unlocked : 1;        //1Bit
	ULONG64 RefCnt : 16;       //16Bits
	ULONG64 Attributes : 3;        //3Bits
	ULONG64 ObjectPointerBits : 44;       //44Bits
};

typedef struct _HANDLE_TABLE_ENTRY
{
	union                                           //that special class
	{
		ULONG64 VolatileLowValue;                   //Int8B
		ULONG64 LowValue;                           //Int8B
		ULONG64 RefCountField;                      //Int8B
		_HANDLE_TABLE_ENTRY_INFO* InfoTable;        //Ptr64 _HANDLE_TABLE_ENTRY_INFO
		A a;
	};
	union
	{
		ULONG64 HighValue;                          //Int8B
		_HANDLE_TABLE_ENTRY* NextFreeHandleEntry;   //Ptr64 _HANDLE_TABLE_ENTRY
	};
} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE
{
	ULONG NextHandleNeedingPool;
	long ExtraInfoPages;
	LONG_PTR TableCode;
	PEPROCESS QuotaProcess;
	LIST_ENTRY HandleTableList;
	ULONG UniqueProcessId;
	ULONG Flags;
	EX_PUSH_LOCK HandleContentionEvent;
	EX_PUSH_LOCK HandleTableLock;
	// More fields here...
} HANDLE_TABLE, * PHANDLE_TABLE;

typedef PHANDLE_TABLE_ENTRY(*pExpLookupHandleTableEntry)(
	IN PVOID64 HandleTable,
	IN HANDLE Handle
	);



typedef BOOLEAN(*pExLockHandleTableEntry)(
	PHANDLE_TABLE HandleTable,
	PHANDLE_TABLE_ENTRY HandleTableEntry
	);

typedef VOID(*pExUnlockHandleTableEntry)(
	PHANDLE_TABLE HandleTable,
	PHANDLE_TABLE_ENTRY HandleTableEntry
	);


typedef HANDLE(*pExCreateHandle)(IN PHANDLE_TABLE 	HandleTable,
	IN PHANDLE_TABLE_ENTRY 	HandleTableEntry
	);

typedef BOOLEAN(*pExDestroyHandle)(IN PHANDLE_TABLE 	HandleTable,
	IN HANDLE 	Handle,
	IN PHANDLE_TABLE_ENTRY HandleTableEntry 	OPTIONAL
	);


extern "C" NTSTATUS ZwQuerySystemInformation(
	_In_      ULONG  SystemInformationClass,
	_Inout_   PVOID                    SystemInformation,
	_In_      ULONG                    SystemInformationLength,
	_Out_opt_ PULONG                   ReturnLength
);

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

// handle table information
typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;