#include "include\ObjectDir.h"

#include "..\RAII\include\RAIIBuffer.h"
#include "..\NTDLL\include\NTDLL.h"
#include "..\Common\include\Error.h"


#pragma comment(lib, "ntdll.lib")


#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)


bool ObjectDir::Create(_In_ std::wstring Name) {
	bool res = true;

	do {
		UNICODE_STRING obj;
		NTDLL::GetInstance().RtlInitUnicodeString(
			&obj, 
			Name.c_str()
		);
		OBJECT_ATTRIBUTES attr;
		InitializeObjectAttributes(
			&attr, 
			&obj, 
			OBJ_CASE_INSENSITIVE, 
			nullptr, 
			nullptr
		);

		HANDLE handle;
		NTSTATUS status = NTDLL::GetInstance().NtCreateDirectoryObject(
			&handle, 
			DIRECTORY_ALL_ACCESS, 
			&attr
		);
		if (status != 0) {
			ERROR_LOG("Failed to create directory object");
			res = false;
			break;
		}

		m_Handle = handle;
	} while (false);

	return res;
}


HANDLE ObjectDir::GetHandle() {
	return m_Handle;
}
