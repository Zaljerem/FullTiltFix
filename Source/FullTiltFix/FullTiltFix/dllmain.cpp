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


static bool g_windowFixed = false;

void CenterGameWindow(HWND hWnd)
{
    RECT rc = {};

    if (!GetWindowRect(hWnd, &rc))
        return;

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenW - width) / 2;
    int y = (screenH - height) / 2;

    SetWindowPos(
        hWnd,
        HWND_TOP,
        x,
        y,
        0,
        0,
        SWP_NOSIZE | SWP_NOACTIVATE);

#ifdef _DEBUG
    char buffer[256];

    sprintf_s(
        buffer,
        "Centered Full Tilt window to %d,%d (%dx%d)\n",
        x,
        y,
        width,
        height);

    OutputDebugStringA(buffer);
#endif
}

DWORD WINAPI WindowFixThread(LPVOID)
{
    // Allow DirectDraw/game startup to finish
    Sleep(250); 

    for (int i = 0; i < 40; i++)
    {
        HWND hWnd = GetForegroundWindow();

        if (hWnd)
        {
            char title[256] = {};
            GetWindowTextA(hWnd, title, sizeof(title));

            if (strstr(title, "Full Tilt!"))
            {
                if (!g_windowFixed)
                {
                    g_windowFixed = true;

                    CenterGameWindow(hWnd);

#ifdef _DEBUG
                    OutputDebugStringA("Applied delayed Full Tilt centering fix\n");
#endif
                }

                break;
            }
        }

        Sleep(100);
    }

    return 0;
}

// ----------------------------------------------------
// Registry hook
// ----------------------------------------------------
LSTATUS WINAPI Hook_RegSetValueExA(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    const BYTE* lpData,
    DWORD cbData)
{
    if (dwType == REG_SZ && lpData && cbData > 0)
    {
        const char* str = (const char*)lpData;

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



// ----------------------------------------------------
// Dll entry + hook install
// ----------------------------------------------------
BOOL APIENTRY DllMain(
    HMODULE hModule,
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

        CreateThread(
            NULL,
            0,
            WindowFixThread,
            NULL,
            0,
            NULL);
    }

    return TRUE;
}