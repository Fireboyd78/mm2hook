#include "common.h"
#include "util.h"

float stopwatch::ticksToSeconds = 0.0f;
float stopwatch::ticksToMilliseconds = 0.0f;

//
// Debugging utilities
//

void debugf(const char *format, ...) {
    char buf[255];
    va_list va;
    va_start(va, format);
    _vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);
    OutputDebugStringA(buf);
}

//
// Empty function templates
//

NAKED void NullSub(void) {
    __asm {
        retn
    }
};

NAKED int ReturnNullOrZero(void) {
    __asm {
        xor eax, eax
        retn
    }
};

NAKED bool ReturnFalse(void) {
    __asm {
        xor al, al
        retn
    }
};

//
// Useful helper functions
//

bool LoadSystemLibrary(LPCSTR lpDllName, HMODULE *out)
{
    char szDllFile[MAX_PATH]{ NULL }; // path to system dll

    GetSystemDirectory(szDllFile, sizeof(szDllFile));
    sprintf(szDllFile, "%s\\%s", szDllFile, lpDllName);

    if (SUCCEEDED(*out = LoadLibrary(szDllFile)))
    {
        LogFile::Format("Loaded system library \"%s\" => %08X\n", szDllFile, *out);
        return true;
    }
    else
    {
        LogFile::Format("System library \"%s\" not found!\n", szDllFile);
        return false;
    }
};

bool GetHookProcAddress(HMODULE hModule, LPCSTR lpProcName, FARPROC *out)
{
    if (SUCCEEDED(*out = GetProcAddress(hModule, lpProcName)))
    {
        LogFile::Format("Found hook proc \"%s\" in module %08X => %08X\n", lpProcName, hModule, *out);
        return true;
    }
    else
    {
        LogFile::Format("Hook proc \"%s\" not found in module %08X!\n", lpProcName, hModule);
        return false;
    }
};