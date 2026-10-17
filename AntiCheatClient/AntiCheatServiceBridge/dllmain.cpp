#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        DebugLogInit();
        break;
    case DLL_PROCESS_DETACH:
        ReleaseDebugLog();
        break;
    }
    return TRUE;
}
