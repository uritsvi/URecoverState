#pragma once

#include <ntddk.h>

#include "..\Common\include\Common.h"

class HandlesTable
{
public:
	HandlesTable();
	~HandlesTable();

	NTSTATUS ChnageHandle(_In_ ChangeHandleParams* Params);
};

