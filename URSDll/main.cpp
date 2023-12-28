#include <Windows.h>

#include "URSDll.h"

#pragma comment(lib, "RAII.lib")
#pragma comment(lib, "Common.lib")

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
        res = g_URSDll.Init();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
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
}