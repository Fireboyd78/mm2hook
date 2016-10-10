#include "common.h"

#include <ctime>

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

const char* FormatTime(const char* format)
{
    static char buffer[1024];

    std::time_t t = time(NULL);

    std::strftime(buffer, sizeof(buffer), format, std::localtime(&t));

    return buffer;
}

const char* GUIDToString(GUID* guid)
{
    if (guid == nullptr)
    {
        return "00000000-0000-0000-0000-000000000000";
    }

    static char buffer[(sizeof(GUID) * 2) + 4 + 1]; // 37 characters: 00000000-0000-0000-0000-000000000000 plus null

    sprintf_s(buffer,
              "%.8X"
              "-%.4X"
              "-%.4X"
              "-%.2X%.2X"
              "-%.2X%.2X%.2X%.2X%.2X%.2X",
              guid->Data1,
              guid->Data2,
              guid->Data3,
              guid->Data4[0], guid->Data4[1],
              guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);

    return buffer;
}