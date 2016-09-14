#include "loader.h"

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
// (Apparently DirectInputCreateW gets called sometimes...)
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreateA_Impl")
#pragma comment(linker, "/EXPORT:DirectInputCreateW=_DirectInputCreateA_Impl")
HRESULT NAKED DirectInputCreateA_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter)
{
    _asm jmp dword ptr ds:lpDICreate
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            debug("Attempting to hook into MM2...");

            // Initialize the log file
            LogFile::Initialize("mm2loader.log", "--<< MM2Hook log file >>--\n");
            LogFile::WriteLine("Loader initialized.");

            HMODULE hDIModule = NULL;

            if (LoadSystemLibrary("dinput.dll", &hDIModule) &&
                GetHookProcAddress(hDIModule, "DirectInputCreateA", (FARPROC*)&lpDICreate))
            {
                LogFile::WriteLine("Loader has successfully injected into DirectInputCreateA.");

                /*
                    TODO: Parse DLLs and look for compatible MM2Hook plugins.
                */
            }
            else
            {
                LogFile::WriteLine("Loader injection FAILED.");
            }

            LogFile::Close();
        } break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}