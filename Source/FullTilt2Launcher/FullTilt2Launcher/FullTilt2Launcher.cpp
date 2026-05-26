#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

bool InjectDLL(DWORD processId, const char* dllPath)
{
    HANDLE hProcess = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        processId);

    if (!hProcess)
        return false;

    size_t len = strlen(dllPath) + 1;

    LPVOID remoteMemory = VirtualAllocEx(
        hProcess,
        NULL,
        len,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);

    if (!remoteMemory)
    {
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(
        hProcess,
        remoteMemory,
        dllPath,
        len,
        NULL))
    {
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");

    FARPROC loadLibraryAddr =
        GetProcAddress(hKernel32, "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)loadLibraryAddr,
        remoteMemory,
        0,
        NULL);

    if (!hThread)
    {
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    // ------------------------------------------------
    // Determine target EXE
    // ------------------------------------------------

    const char* possibleTargets[] =
    {
        "HERO.EXE",
        "MAD.EXE",
        "ALIEN.EXE"
    };

    const char* selectedExe = nullptr;

    for (const char* exe : possibleTargets)
    {
        DWORD attrib = GetFileAttributesA(exe);

        if (attrib != INVALID_FILE_ATTRIBUTES)
        {
            selectedExe = exe;
            break;
        }
    }

    if (!selectedExe)
    {
        MessageBoxA(
            NULL,
            "Could not find ALIEN.EXE, HERO.EXE, or MAD.EXE",
            "Full Tilt 2 Launcher",
            MB_ICONERROR);

        return 1;
    }

    // ------------------------------------------------
    // Build DLL path
    // ------------------------------------------------

    char dllPath[MAX_PATH] = {};

    GetFullPathNameA(
        "FullTilt2Fix.dll",
        MAX_PATH,
        dllPath,
        NULL);

    // ------------------------------------------------
    // Launch suspended
    // ------------------------------------------------

    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};

    si.cb = sizeof(si);

    BOOL created = CreateProcessA(
        selectedExe,
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi);

    if (!created)
    {
        MessageBoxA(
            NULL,
            "Failed to launch game.",
            "Full Tilt 2 Launcher",
            MB_ICONERROR);

        return 1;
    }

    // ------------------------------------------------
    // Inject DLL
    // ------------------------------------------------

    if (!InjectDLL(pi.dwProcessId, dllPath))
    {
        MessageBoxA(
            NULL,
            "DLL injection failed.",
            "Full Tilt 2 Launcher",
            MB_ICONERROR);

        TerminateProcess(pi.hProcess, 1);

        return 1;
    }

    // ------------------------------------------------
    // Resume game
    // ------------------------------------------------

    ResumeThread(pi.hThread);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}