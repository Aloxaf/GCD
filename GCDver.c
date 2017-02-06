#include "GCD.h"

DLL_EXPORT void Version(void)
{
    puts("GCD.dll 1.1 by aiwozhonghuaba\n");
}
/*
bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    HANDLE g_hModule;
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        puts("GCD.dll is attached!");
        g_hModule = (HINSTANCE)hModule;
        break;
    case DLL_PROCESS_DETACH:
        puts("GCD.dll is detached!");
        g_hModule = NULL;
        break;
    }
    return true;
}*/
