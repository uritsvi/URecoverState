#include "DmpFile.h"
#include "Error.h"
#include "TargetProcess.h"

#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib");

#define DMP_FILE_NAME "state.dmp"

#define MINI_DUMP_FLAGS (MINIDUMP_TYPE)\
						(MiniDumpWithFullMemory|\
						MiniDumpWithHandleData|\
						MiniDumpWithModuleHeaders|\
						MiniDumpWithUnloadedModules|\
					    MiniDumpWithProcessThreadData|\
						MiniDumpWithFullMemoryInfo|\
						MiniDumpWithThreadInfo)


bool DmpFile::Dump(_In_ std::shared_ptr<RAIIDirectory> TargetDirectory) {
	bool res = true;

	do {
		std::string path;
		TargetDirectory->MakePath(
			DMP_FILE_NAME, 
			path
		);

		res = m_File.Create(path, true);
		if (!res) {
			ERROR_LOG("Failed to create dmp file");
			break;
		}


		auto targetProcess =
			TargetProcess::GetInstance().GetProcess();
		auto targetId =
			targetProcess->GetPID();

		res = MiniDumpWriteDump(
			targetProcess->GetHandle(),
			targetId,
			m_File.GetHandle(),
			MINI_DUMP_FLAGS,
			nullptr,
			nullptr,
			nullptr
		);
		if (!res) {
			ERROR_LOG("Failed to generate a mini dump");
			break;
		}

	} while (false);

	return res;
}

bool DmpFile::Read(_In_ StateInfo& Info) {
	bool res = true;

	do {
		auto& mapping = Info.Mapping;
		res = mapping.Create(
			m_File,
			m_File.GetSize()
		);
		if (!res) {
			ERROR_LOG("Failed to create file mapping");
			break;
		}
		auto addr = mapping.GetMappedAddr();

			PMINIDUMP_DIRECTORY dir;
			ULONG size;


			res = MiniDumpReadDumpStream(
				addr,
				ThreadListStream,
				&dir,
				(LPVOID*)&Info.ThreadInfo,
				&size
			);
			if (!res) {
				ERROR_LOG("Failed to read minidump threads state");
				break;
			}
			res = MiniDumpReadDumpStream(
				addr,
				MemoryInfoListStream,
				&dir,
				(LPVOID*)&Info.MemInfo,
				&size
			);
			if (!res) {
				ERROR_LOG("Failed to read minidump memory info state");
				break;
			}
			res = MiniDumpReadDumpStream(
				addr,
				Memory64ListStream,
				&dir,
				(LPVOID*)&Info.Mem,
				&size
			);
			if (!res) {
				ERROR_LOG("Failed to read minidump memory state");
				break;
			}
			res = MiniDumpReadDumpStream(
				addr,
				HandleDataStream,
				&dir,
				(LPVOID*)&Info.HandlesInfo,
				&size
			);
			if (!res) {
				ERROR_LOG("Failed to read minidump handles state");
				break;
			}
			res = MiniDumpReadDumpStream(
				addr,
				ModuleListStream,
				&dir,
				(LPVOID*)&Info.ModulesInfo,
				&size
			);
			if (!res) {
				ERROR_LOG("Failed to read minidump modules state");
				break;
			}

		} while (false);

	return res;
}