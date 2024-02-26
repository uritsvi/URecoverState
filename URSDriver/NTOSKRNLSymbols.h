#pragma once

#include <Windows.h>
#include <string>
#include "../Common/include/Common.h"
#include "..\RAII\include\RAIIDirectory.h"

class NTOSKRNLSymbols
{
public:
	NTOSKRNLSymbols();
	~NTOSKRNLSymbols();

	bool Init(_In_ KernelInfo& KernelInfo);

	bool FindAllSymbols(_Out_ KernelFunctions& Symbols);

private:
	bool FindSymbolAddr(
		_In_ std::string Name,
		_Out_ std::list<std::string>& NotFound,
		_Out_ bool& Found, 
		_Out_ LPVOID& Addr
	);
	bool LoadOrDownloadPdb(_Out_ std::string& NTOSKRNLPdbPath);
	bool DownloadPdb(RAIIDirectory& SymbolsDir);

private:
	static UINT m_Instance;
};

