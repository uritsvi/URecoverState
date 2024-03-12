#include "TestRun.h"
#include "..\Common\include\Error.h"

#include "..\Common\include\Common.h"
#include "..\ntdll\include\ntdll.h"
#include "..\raii\include\raiithread.h"
#include "..\raii\include\raiievent.h"
#include "..\raii\include\raiihandle.h"

#include "..\URS\include\URSInterface.h"

#include "Breakpoint.h"

#include <Psapi.h>
#include <vector>

#include <string>
#include <list>
#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <Psapi.h>
#include <algorithm>
#include <iterator>

#pragma comment(lib, "urs.lib")
#pragma comment(lib, "raii.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "psapi.lib")

#define TESTS_PROCESS_NAME "Tests.exe"

#define WAIT_FOR_EVENTS 500

#define NOP_INSTRUCTION 0x90

std::shared_ptr<RAIIProcess> g_TargetProcess;

Breakpoint g_MainFuncBp;
RAIIEvent g_DeatachEvent;
std::shared_ptr<RAIIThread> g_DebuggerThread;

std::list<LPVOID> toDisable;

namespace TestRun {

	struct module_data {
		std::string image_name;
		std::string module_name;
		void* base_address;
		DWORD load_size;
	};


	DWORD DumpStackTrace(EXCEPTION_POINTERS* ep);




	class symbol {
		typedef IMAGEHLP_SYMBOL64 sym_type;
		sym_type* sym;
		static const int max_name_len = 1024;

	public:
		symbol(HANDLE process, DWORD64 address) : sym((sym_type*)::operator new(sizeof(*sym) + max_name_len)) {
			memset(sym, '\0', sizeof(*sym) + max_name_len);
			sym->SizeOfStruct = sizeof(*sym);
			sym->MaxNameLength = max_name_len;
			DWORD64 displacement;

			SymGetSymFromAddr64(process, address, &displacement, sym);
		}

		std::string name() { return std::string(sym->Name); }
		std::string undecorated_name() {
			if (*sym->Name == '\0')
				return "<couldn't map PC to fn name>";
			std::vector<char> und_name(max_name_len);
			UnDecorateSymbolName(sym->Name, &und_name[0], max_name_len, UNDNAME_COMPLETE);
			return std::string(&und_name[0], strlen(&und_name[0]));
		}
	};


	class get_mod_info {
		HANDLE process;
		static const int buffer_length = 4096;
	public:
		get_mod_info(HANDLE h) : process(h) {}

		module_data operator()(HMODULE module) {
			module_data ret;
			char temp[buffer_length];
			MODULEINFO mi;

			GetModuleInformation(process, module, &mi, sizeof(mi));
			ret.base_address = mi.lpBaseOfDll;
			ret.load_size = mi.SizeOfImage;

			GetModuleFileNameExA(process, module, temp, sizeof(temp));
			ret.image_name = temp;
			GetModuleBaseNameA(process, module, temp, sizeof(temp));
			ret.module_name = temp;
			std::vector<char> img(ret.image_name.begin(), ret.image_name.end());
			std::vector<char> mod(ret.module_name.begin(), ret.module_name.end());
			SymLoadModule64(process, 0, &img[0], &mod[0], (DWORD64)ret.base_address, ret.load_size);
			return ret;
		}
		};


	// if you use C++ exception handling: install a translator function
	// with set_se_translator(). In the context of that function (but *not*
	// afterwards), you can either do your stack dump, or save the CONTEXT
	// record as a local copy. Note that you must do the stack dump at the
	// earliest opportunity, to avoid the interesting stack-frames being gone
	// by the time you do the dump.
	DWORD DumpStackTrace(EXCEPTION_RECORD* ep, CONTEXT* context, HANDLE hThread, HANDLE process)
	{
		int frame_number = 0;
		DWORD offset_from_symbol = 0;
		IMAGEHLP_LINE64 line = { 0 };
		std::vector<module_data> modules;
		DWORD cbNeeded;
		std::vector<HMODULE> module_handles(1);

		// Load the symbols:
		// WARNING: You'll need to replace <pdb-search-path> with either NULL
		// or some folder where your clients will be able to find the .pdb file.
		if (!SymInitialize(process, nullptr, false))
			throw(std::logic_error("Unable to initialize symbol handler"));
		DWORD symOptions = SymGetOptions();
		symOptions |= SYMOPT_LOAD_LINES | SYMOPT_UNDNAME;
		SymSetOptions(symOptions);
		EnumProcessModules(process, &module_handles[0], module_handles.size() * sizeof(HMODULE), &cbNeeded);
		module_handles.resize(cbNeeded / sizeof(HMODULE));
		EnumProcessModules(process, &module_handles[0], module_handles.size() * sizeof(HMODULE), &cbNeeded);
		std::transform(module_handles.begin(), module_handles.end(), std::back_inserter(modules), get_mod_info(process));
		void* base = modules[0].base_address;

		// Setup stuff:

#ifdef _M_X64
		STACKFRAME64 frame;
		frame.AddrPC.Offset = context->Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context->Rsp;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context->Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
#else
		STACKFRAME64 frame;
		frame.AddrPC.Offset = context->Eip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context->Esp;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context->Ebp;
		frame.AddrFrame.Mode = AddrModeFlat;
#endif

		int n = 0;
		// Build the string:
		std::ostringstream builder;
		do {
			if (frame.AddrPC.Offset != 0) {
				std::string fnName = symbol(process, frame.AddrPC.Offset).undecorated_name();
				builder << fnName;
				if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &offset_from_symbol, &line))
					builder << "  " /*<< line.FileName*/ << "(" << line.LineNumber << ")\n";
				else builder << "\n";
				if (fnName == "main")
					break;
				if (fnName == "RaiseException") {
					// This is what we get when compiled in Release mode:
					return EXCEPTION_EXECUTE_HANDLER;
				}
			}
			else
				builder << "(No Symbols: PC == 0)";
			if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, hThread, &frame, context, NULL,
				SymFunctionTableAccess64, SymGetModuleBase64, NULL))
				break;
		} while (frame.AddrReturn.Offset != 0);
		//return EXCEPTION_EXECUTE_HANDLER;
		SymCleanup(process);

		std::cout << builder.str() << "\n";

		// Display the string:
		return EXCEPTION_EXECUTE_HANDLER;
	}


	void Run() {

		bool res = true;

		do {
			g_TargetProcess = std::make_shared<RAIIProcess>();

			res = g_DeatachEvent.Create(false);
			if (!res) {
				ERROR_LOG("Failed to create event");
				break;
			}

			res = NTDLL::GetInstance().Init();
			if (!res) {
				ERROR_LOG("Failed to init ntdll");
				break;
			}

			res = g_TargetProcess->Create(
				TESTS_PROCESS_NAME,
				"",
				DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE
			);
			if (!res) {
				ERROR_LOG("Failed to create target process");
				break;
			}

			res = DebuggerLoop();
			if (!res) {
				ERROR_LOG("Failed to run debugger loop");
				break;
			}

		} while (false);

		if (!res) {
			ERROR_LOG("Failed to run all tests");
		}
	}

	bool DetachDebugger() {
		bool res = true;

		do {
			res = g_DeatachEvent.Set();
			if (!res) {
				ERROR_LOG("Failed to set deatach event");
				break;
			}
			
			DWORD exitCode;
			res = g_DebuggerThread->WaitForTerminate(exitCode);
			if (!res){
				ERROR_LOG("Failed to wait for terminate");
				break;
			}

			g_TargetProcess->Close();

		} while (false);

		return res;
	}
	bool ReatchDebugger(std::shared_ptr<RAIIProcess> NewTarget) {
		bool res = true;

   		g_TargetProcess = NewTarget;
	


		do {
			for (auto b : toDisable) {
				res = DisableBreakpoint(b);
				if (!res) {
					ERROR_LOG("ERROR");
					break;
				}
			}

			res = DebugActiveProcess(g_TargetProcess->GetPID());
			if (!res) {
				ERROR_LOG("Failed to reatach to process");
				break;
			}

			SUCCESS_LOG("Reatach to process successfully");
		
			res = g_TargetProcess->Resume();
			if (!res) {
				ERROR_LOG("Failed to resume process");
				break;
			}
			res = g_TargetProcess->Resume();
			res = g_TargetProcess->Resume();
			res = g_TargetProcess->Resume();
			if (!res) {
				ERROR_LOG("Failed to resume process");
				break;
			}

			res = DebuggerLoop();
			if (!res) {
				ERROR_LOG("Error in debugger loop");
				break;
			}
		} while (false);

		return res;
	}



	bool DebuggerLoop() {
		bool res = true;
		bool firstBp = true;
		DEBUG_EVENT event;
		LPVOID mainFuncAddr = nullptr;

		g_DebuggerThread = std::make_shared<RAIIThread>();
		g_DebuggerThread->Open(GetCurrentThreadId());

		SUCCESS_LOG("New debugger loop");


		while (true) {

			bool resumeThread = true;


			do {
				res = WaitForDebugEvent(
					&event,
					WAIT_FOR_EVENTS
				);
				if (g_DeatachEvent.IsSet()) {
					res = DebugActiveProcessStop(g_TargetProcess->GetPID());
					if (!res) {
						ERROR_LOG("Failed to deatach from process");
						break;
					}
					ExitThread(0);
				}
			}while(!res);


			bool fatal = false;
			bool notHandled = false;
			switch (event.dwDebugEventCode) {
			case EXIT_PROCESS_DEBUG_EVENT: {
				std::cout << "Shut down\n";
			}break;

			case EXCEPTION_DEBUG_EVENT: {
				auto ex = event.u.Exception;
				if (ex.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
					RAIIThread thread;
					res = thread.Open(event.dwThreadId);
					if (!res) {
						ERROR_LOG("Failed to open thread");
						break;
					}
					CONTEXT context = { 0 };
					context.ContextFlags = CONTEXT_ALL;

					thread.GetContext(&context);
					DumpStackTrace(&ex.ExceptionRecord, &context, thread.GetRAIIHandle()->GetHandle(), g_TargetProcess->GetRAIIHandle()->GetHandle());

					std::cout << std::to_string(event.dwThreadId) << "\n";

					ERROR_LOG("Accesss violation addr: " + std::to_string((UINT64)ex.ExceptionRecord.ExceptionAddress));
					fatal = true;
					break;
				}

				if (ex.ExceptionRecord.ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
					ERROR_LOG("Stack overflow xception");
					fatal = true;

					RAIIThread thread;
					res = thread.Open(event.dwThreadId);
					if (!res) {
						ERROR_LOG("Failed to open thread");
						break;
					}
					
					CONTEXT context = {0};
					context.ContextFlags = CONTEXT_ALL;

					thread.GetContext(&context);

					DumpStackTrace(&ex.ExceptionRecord, &context, thread.GetRAIIHandle()->GetHandle(), g_TargetProcess->GetRAIIHandle()->GetHandle());
					if (!res) {
						ERROR_LOG("Failed to list call stack");
					}
					break;
				}

				if(ex.ExceptionRecord.ExceptionCode == STATUS_GUARD_PAGE_VIOLATION){
					notHandled = true;
					break;
				}

				if (ex.ExceptionRecord.ExceptionCode == EXCEPTION_INVALID_HANDLE) {
					break;
				}

				if (ex.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT) {

					CONTEXT context = { 0 };
					context.ContextFlags = CONTEXT_ALL;
					RAIIThread thread;
					res = thread.Open(event.dwThreadId);
					if (!res) {
						ERROR_LOG("Failed to open thread");
						break;
					}
					thread.GetContext(&context);

					DumpStackTrace(&ex.ExceptionRecord, &context, thread.GetRAIIHandle()->GetHandle(), g_TargetProcess->GetRAIIHandle()->GetHandle());

					ERROR_LOG("Unknon exception");
					//fatal = true;
					break;
				}

				SUCCESS_LOG("Bp event!");

				if (firstBp) {
					res = SetBpOnMain(mainFuncAddr);
					if (!res) {
						ERROR_LOG("Failed to set bp on main");
						
						
						;
					}

					firstBp = false;
					break;
				}

				res = g_TargetProcess->Suspend();
				if (!res) {
					ERROR_LOG("Failed to susspend process");
					break;
				}

				if (ex.ExceptionRecord.ExceptionAddress == mainFuncAddr) {


					RAIIThread thread;
					DWORD tid;


					thread.Create(
						[](LPVOID Ctx) -> DWORD {
							bool res = InitURS((DWORD)Ctx, DebuggerFunctions{ DetachDebugger, ReatchDebugger });
								if (!res) {
									ERROR_LOG("Failed to init usr");
								}
								g_TargetProcess->Resume();
								return 0;
							},
							(LPVOID)g_TargetProcess->GetPID(),
							tid);


					RAIIThread targetThread;
					targetThread.Open(event.dwThreadId);


					CONTEXT context = { 0 };
					context.ContextFlags = CONTEXT_ALL;

					targetThread.GetContext(&context);

					context.Rip--;

					targetThread.SetContext(&context);

					res = g_MainFuncBp.Disable();
					if (!res) {
						ERROR_LOG("Failed to disable main func breakpoint");
						break;
					}
					break;
				}

				res = HandleBreakpoint(ex);
				if (!res) {
					ERROR_LOG("Failed to handle bp");
					break;
				}


			}break;
			}

			auto continueType = DBG_CONTINUE;
			if (fatal) {
				continueType = DBG_EXCEPTION_NOT_HANDLED;
			}
			if (notHandled) {
				continueType = DBG_EXCEPTION_NOT_HANDLED;
			}

			res = ContinueDebugEvent(
				event.dwProcessId,
				event.dwThreadId,
				continueType
			);
			if (!res) {
				ERROR_LOG("Failed to continue");
				break;
			}

		}

		return res;

	}

	bool SetBpOnMain(_Out_ LPVOID& BpAddr) {
		bool res = true;

		MODULEINFO info;

		do {
			res = GetModuleInformation(
				g_TargetProcess->GetRAIIHandle()->GetHandle(),
				nullptr,
				&info,
				sizeof(info)
			);
			if (!res) {
				ERROR_LOG("Failed to get module info");
				break;
			}

			BpAddr = info.EntryPoint;

			res = g_MainFuncBp.Create(
				g_TargetProcess,
				BpAddr
			);
			if (!res) {
				ERROR_LOG("Failed to set bp");
				break;
			}
			res = g_MainFuncBp.Enable();
			if (!res) {
				ERROR_LOG("Failed to enuble bp");
				break;
			}
		} while (false);

		return res;
	}

	bool HandleBreakpoint(EXCEPTION_DEBUG_INFO& Event) {
		bool res = true;
		SharedMem sharedMem;
		char* stateName = nullptr;

		do {
			res = ReadProcessMemory(
				g_TargetProcess->GetRAIIHandle()->GetHandle(),
				(LPCVOID)TESTS_SHARED_MEM,
				&sharedMem,
				sizeof(sharedMem),
				nullptr
			);
			if (!res) {
				ERROR_LOG("Failed to read process mem");
				break;
			}
			stateName = (char*)malloc(sharedMem.StateStrLen);
			res = ReadProcessMemory(
				g_TargetProcess->GetRAIIHandle()->GetHandle(),
				(LPCVOID)(TESTS_SHARED_MEM + sizeof(SharedMem)),
				stateName,
				sharedMem.StateStrLen,
				nullptr
			);
			if (!res) {
				ERROR_LOG("Failed to read process mem");
				break;
			}

			std::cout << stateName << "\n";

			RAIIThread thread;
			DWORD tid;
			switch (sharedMem.Op) {
			case TESTS_OP_CAPTURE: {

				thread.Create(
					[](LPVOID Ctx) -> DWORD {
						bool res = URSCaptureState();
				if (!res) {
					ERROR_LOG("Failed to take snapshot");
					return false;
				}
				g_TargetProcess->Resume();

				return true;
					},
					nullptr,
						tid);
			}break;

			case TESTS_OP_REVERT: {


				thread.Create(
					[](LPVOID Ctx) -> DWORD {
						bool res = URSRecoverState();
				if (!res) {
					ERROR_LOG("Failed to revert to snapshot");
					return false;
				}

				g_TargetProcess->Resume();

				return true;
					},
					stateName,
						tid
						);


			}break;


		
			}

			auto bpAddr = Event.ExceptionRecord.ExceptionAddress;
			res = DisableBreakpoint(bpAddr);
			if (!res) {
				ERROR_LOG("Failed to disable breakpoint");
				break;
			}
			toDisable.push_back(bpAddr);

		} while (false);

		if (stateName != nullptr) {
			free(stateName);
		}

		return res;
	}

	bool DisableBreakpoint(_In_ LPVOID Addr) {
		bool res = true;

		do {
			char nopInst = NOP_INSTRUCTION;
			res = g_TargetProcess->WriteMem(
				Addr,
				&nopInst,
				sizeof(nopInst)
			);
		} while (false);

		return res;
	}
}