#include <windows.h>
#include <stdio.h>
#include "MinHook.h"

typedef LSTATUS(WINAPI* RegSetValueExA_t)(
    HKEY,
    LPCSTR,
    DWORD,
    DWORD,
    const BYTE*,
    DWORD);

RegSetValueExA_t Real_RegSetValueExA = nullptr;

LSTATUS WINAPI Hook_RegSetValueExA(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    const BYTE* lpData,
    DWORD cbData)
{
    // Only fix REG_SZ values
    if (dwType == REG_SZ && lpData && cbData > 0)
    {
        const char* str = (const char*)lpData;

        // If terminator is missing, fix length
        if (str[cbData - 1] != '\0')
        {
            cbData++;

#ifdef _DEBUG
            char buffer[512];
            sprintf_s(buffer,
                "Fixed registry string: %s\n",
                lpValueName ? lpValueName : "(null)");
            OutputDebugStringA(buffer);
#endif
        }
    }

    return Real_RegSetValueExA(
        hKey,
        lpValueName,
        Reserved,
        dwType,
        lpData,
        cbData);
}
;


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        if (MH_Initialize() != MH_OK)
            return FALSE;

        if (MH_CreateHookApi(
            L"Advapi32",
            "RegSetValueExA",
            &Hook_RegSetValueExA,
            reinterpret_cast<LPVOID*>(&Real_RegSetValueExA))
            != MH_OK)
        {
            return FALSE;
        }

        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
            return FALSE;
    }

    return TRUE;
}
