#include "NTOSKRNLSymbols.h"
#include "..\Common\include\Error.h"
#include "..\Common\include\Common.h"


#include "..\RAII\include\RAIIProcess.h"

#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp")

#define SYMBOLS_DIR_PATH "C:\\Symbols"
#define NTOSKRNL_EXE "C:\\Windows\\System32\\ntoskrnl.exe"
#define NTOSKRNL_PDB "ntkrnlmp.pdb"


#define PDB_DOWNLOADER "PDBDownloader.exe"

UINT NTOSKRNLSymbols::m_Instance;

NTOSKRNLSymbols::NTOSKRNLSymbols() {
	(UINT64)m_Instance++;
}
NTOSKRNLSymbols::~NTOSKRNLSymbols() {
}

bool NTOSKRNLSymbols::Init(_In_ KernelInfo& KernelInfo) {

	bool res = true;
	HANDLE instance = (HANDLE)m_Instance;

	do {
		res = SymInitialize(
			instance,
			nullptr,
			FALSE
		);
		if (!res) {
			ERROR_LOG("Failed to init sym");
			break;
		}
		std::string pdbPath;
		res = LoadOrDownloadPdb(pdbPath);
		if (!res) {
			ERROR_LOG("Failed to download symbols");
			break;
		}
		res = SymLoadModule64(
			instance,
			nullptr,
			pdbPath.c_str(),
			nullptr,
			(DWORD64)KernelInfo.BaseAddr,
			KernelInfo.Size
		);
		if (!res) {
			ERROR_LOG("Failed to load ntoskrnl symbols");
			break;
		}
	} while (false);

	return res;
}

/*
* And the value of found 
*/
bool NTOSKRNLSymbols::FindSymbolAddr(
	_In_ std::string Name,
	_Out_ std::list<std::string>& NotFound,
	_Out_ bool& Found,
	_Out_ LPVOID& Addr) {

	bool res = true;
	HANDLE instance = (HANDLE)m_Instance;

	do {
		SYMBOL_INFO symbol = { 0 };
		symbol.SizeOfStruct = sizeof(SYMBOL_INFO);

		bool found = SymFromName(
			instance,
			Name.c_str(),
			&symbol
		);
		if (!found) {
			ERROR_LOG("Symbol not found");
			break;
		}
		Found &= found;

		Addr = (LPVOID)symbol.Address;
	} while (false);
	if (!res) {
		NotFound.push_back(Name);
		Found &= false;
	}

	return res;
}

bool NTOSKRNLSymbols::LoadOrDownloadPdb(_Out_ std::string& NTOSKRNLPdbPath) {
	bool res = true;

	RAIIDirectory symbols;
	RAIIDirectory subDir;

	while (true) {
		/*
		* Cant enter the first time the loop run
		*/
		if (!res) {
			res = DownloadPdb(symbols);
			if (!res) {
				ERROR_LOG("Failed to download pdb");
				break;
			}
		}

		res = symbols.Open(SYMBOLS_DIR_PATH);
		if (!res) {
			res = symbols.Create(SYMBOLS_DIR_PATH, false);
			if (!res) {
				ERROR_LOG("Failed to create symbols dir");
				break;
			}
		}
		res = symbols.OpenSubDir(
			NTOSKRNL_PDB,
			false,
			subDir
		);
		if (!res) {
			WARNING_LOG("Failed to open sub dir");
			continue;
		}
		res = subDir.OpenSubDir(
			"",
			true,
			subDir
		);
		if (!res) {
			WARNING_LOG("Failed to open subdir");
			continue;
		}
		std::list<std::string> pdbs;
		res = subDir.FilesWithExtension(
			".pdb",
			pdbs
		);
		if (!res) {
			ERROR_LOG("Failed to find files with extension pdb");
			continue;
		}
		if (pdbs.size() > 1 || pdbs.size() == 0) {
			ERROR_LOG("Find more then one or zero pdbs");
			continue;
		}

		NTOSKRNLPdbPath = pdbs.front();
		break;
	}

	return res;
}

bool NTOSKRNLSymbols::DownloadPdb(RAIIDirectory& SymbolsDir) {
	bool res = true;

	RAIIDirectory subDir;


	do {
		RAIIProcess process;
		res = process.Create(
			PDB_DOWNLOADER,
			NTOSKRNL_EXE,
			0
		);
		if (!res) {
			ERROR_LOG("Failed to create pdb downloader process");
			break;
		}

		DWORD exitCode;
		res = process.WaitForExit(exitCode);
		if (!res) {
			ERROR_LOG("Failed to wait for exit");
			break;
		}

		if (exitCode != 0) {
			res = false;
			ERROR_LOG("Failed to download symbols");
			break;
		}
		res = SymbolsDir.OpenSubDir(
			NTOSKRNL_PDB,
			false,
			subDir
		);
		if (!res) {
			ERROR_LOG("Failed to open dir");
			break;
		}
		res = subDir.SetDeleteOnReboot();
		if (!res) {
			ERROR_LOG("Failed to set move file option");
			break;
		}
	} while (false);

	return res;
}

bool NTOSKRNLSymbols::FindAllSymbols(_Out_ KernelFunctions& Symbols) {
	bool res = true;
	bool foundAll = true;
	std::list<std::string> notFound;

#define PSP_CID_TABLE "PspCidTable"
#define EXP_LOOKUP_HANDLE_TABLE_ENTRY "ExpLookupHandleTableEntry"
#define EX_LOCK_HANDLE_TABLE_ENTRY "ExLockHandleTableEntry"
#define EX_UNLOCK_HANDLE_TABLE_ENTRY "ExUnlockHandleTableEntry"
#define EX_DESTROY_HANDLE "ExDestroyHandle"
#define EX_CREATE_HANDLE "ExCreateHandle"

	do {
		Symbols.NumOfKernelFunction = 
			(sizeof(KernelFunctions) - sizeof(SIZE_T)) / sizeof(PVOID);

		FindSymbolAddr(
			PSP_CID_TABLE,
			notFound,
			foundAll,
			Symbols.pPspCidTable
		);
		FindSymbolAddr(
			EXP_LOOKUP_HANDLE_TABLE_ENTRY,
			notFound,
			foundAll,
			Symbols.ExpLookupHandleTableEntry
		);
		FindSymbolAddr(
			EX_LOCK_HANDLE_TABLE_ENTRY,
			notFound,
			foundAll,
			Symbols.ExLockHandleTableEntry
		);
		FindSymbolAddr(
			EX_UNLOCK_HANDLE_TABLE_ENTRY,
			notFound,
			foundAll,
			Symbols.ExUnlockHandleTableEntry
		);
		FindSymbolAddr(
			EX_DESTROY_HANDLE,
			notFound,
			foundAll,
			Symbols.ExDestroyHandle
		);
		FindSymbolAddr(
			EX_CREATE_HANDLE,
			notFound,
			foundAll,
			Symbols.ExCreateHandle
		);
		if (!foundAll) {
			for (auto item : notFound) {
				ERROR_LOG(std::string("Symbol ") + item + " not found");
			}
			res = false;
		}
		std::cout << Symbols.pPspCidTable << "\n";
		std::cout << Symbols.NumOfKernelFunction << "\n";



	} while (false);

	return res;
}