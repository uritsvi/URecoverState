#include <Windows.h>

#include "URSDll.h"
#include "..\NTDLL\include\NTDLL.h"
#include "..\Common\include\Error.h"


#pragma comment(lib, "RAII.lib")
#pragma comment(lib, "Common.lib")
#pragma comment(lib, "NTDLL.lib")

URSDll g_URSDll;

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    BOOL res = true;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:

        res = NTDLL::GetInstance().Init();
        if (!res) {
            ERROR_LOG("Failed to init ntdll");
            break;
        }

        res = g_URSDll.Init();
        break;

    case DLL_THREAD_ATTACH:
        std::cout << "hello";
        break;

    case DLL_THREAD_DETACH:
        std::cout << "hello";

        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
            break;
        }

        res = g_URSDll.Shutdown();

        break;
    }
    
    return res;


    return true;
}