#include <windows.h>
#include <stdio.h>


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
        "Centered Full Tilt 2 window to %d,%d (%dx%d)\n",
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

            if (strstr(title, "Full Tilt! 2"))
            {
                if (!g_windowFixed)
                {
                    g_windowFixed = true;

                    CenterGameWindow(hWnd);

#ifdef _DEBUG
                    OutputDebugStringA("Applied delayed Full Tilt 2 centering fix\n");
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