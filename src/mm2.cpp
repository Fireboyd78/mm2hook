#include "mm2.h"

using namespace MM2;

#define DEFINE_PRINT_HOOK(x,y) \
NAKED void x(LPCSTR str, ...) { \
    __asm jmp dword ptr ds:y \
}

MM2Version EngineVersionToGameVersion(int engineVersion) {
    switch (engineVersion) {
        case 3323:
            return MM2_BETA_1;
        case 3366:
            return MM2_BETA_2;
        case 3393:
            return MM2_RETAIL;
    }
    return MM2_INVALID;
}

FnPtr<void>     lpPrintf;
FnPtr<void>     lpMessagef;
FnPtr<void>     lpDisplayf;
FnPtr<void>     lpWarningf;
FnPtr<void>     lpErrorf;
FnPtr<void>     lpQuitf;
FnPtr<void>     lpAbortf;

FnPtr<void>     lpStream_DumpOpenFiles;
FnPtr<Stream*>  lpStream_Open;
FnPtr<Stream*>  lpStream_Create;
FnPtr<int>      lpStream_Read;
FnPtr<int>      lpStream_Write;
FnPtr<int>      lpStream_GetCh;
FnPtr<int>      lpStream_PutCh;
FnPtr<int>      lpStream_Seek;
FnPtr<int>      lpStream_Tell;

FnPtr<void>     lpdatOutput_CloseLog;
FnPtr<bool>     lpdatOutput_OpenLog;
FnPtr<void>     lpdatOutput_SetOutputMask;

static const MM2InitData _funcs_init[] = {
    // IMPORTANT: New versions must be reflected here!
    // FUNCTION                                 ADDRESS [BETA 1/BETA 2/RETAIL]

    &lpdatOutput_CloseLog,                      { NULL, NULL, 0x4C9530 },
    &lpdatOutput_SetOutputMask,                 { NULL, NULL, 0x4C9590 },
    &lpdatOutput_OpenLog,                       { NULL, NULL, 0x4C95A0 },

    &lpPrintf,                                  { NULL, NULL, 0x4C9720 },
    &lpMessagef,                                { NULL, NULL, 0x4C9750 },
    &lpDisplayf,                                { NULL, NULL, 0x4C9780 },
    &lpWarningf,                                { NULL, NULL, 0x4C97B0 },
    &lpErrorf,                                  { NULL, NULL, 0x4C97E0 },
    &lpQuitf,                                   { NULL, NULL, 0x4C9810 },
    &lpAbortf,                                  { NULL, NULL, 0x4C9850 },

    &lpStream_DumpOpenFiles,                    { NULL, NULL, 0x4C9970 },
    &lpStream_Open,                             { NULL, NULL, 0x4C99C0 },
    &lpStream_Create,                           { NULL, NULL, 0x4C9A00 },
    &lpStream_Read,                             { NULL, NULL, 0x4C9AA0 },
    &lpStream_Write,                            { NULL, NULL, 0x4C9BF0 },
    &lpStream_GetCh,                            { NULL, NULL, 0x4C9D00 },
    &lpStream_PutCh,                            { NULL, NULL, 0x4C9D30 },
    &lpStream_Seek,                             { NULL, NULL, 0x4C9D60 },
    &lpStream_Tell,                             { NULL, NULL, 0x4C9DB0 },

    NULL // must be last
};

int InitializeMM2(MM2Version gameVersion) {
    const MM2InitData *func;
    int nFuncs = 0;

    LogFile::WriteLine("Initializing MM2 hooks...");

    for (func = _funcs_init; func->lpHook; func++, nFuncs++) {
        auto addr = func->addresses[gameVersion];

        if (addr != NULL)
            *func->lpHook = addr;
    }

    LogFile::Format(" - nFuncs: %d\n", nFuncs);

    return (nFuncs > 0) ? HOOK_INIT_OK : HOOK_INIT_UNSUPPORTED;
}

CMidtownMadness2::CMidtownMadness2(int engineVersion)
    : CAGEGame(engineVersion) {
    this->m_gameVersion = EngineVersionToGameVersion(engineVersion);
    
    // try to initialize manager and hooks
    if (this->Initialize() != HOOK_INIT_OK ||
        InitializeMM2(this->m_gameVersion) != HOOK_INIT_OK) {
        LogFile::WriteLine("Failed to initialize the hook system! Terminating...");

        MessageBox(NULL, "A fatal error occurred while trying to initialize MM2Hook. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);

        ExitProcess(EXIT_FAILURE);
    }
}

CMidtownMadness2::CMidtownMadness2(LPAGEGameInfo gameEntry)
    : CMidtownMadness2(gameEntry->age_version) {
}

namespace MM2 {
    DEFINE_PRINT_HOOK(Printf, lpPrintf);
    DEFINE_PRINT_HOOK(Messagef, lpMessagef);
    DEFINE_PRINT_HOOK(Displayf, lpDisplayf);
    DEFINE_PRINT_HOOK(Warningf, lpWarningf);
    DEFINE_PRINT_HOOK(Errorf, lpErrorf);
    DEFINE_PRINT_HOOK(Quitf, lpQuitf);
    DEFINE_PRINT_HOOK(Abortf, lpAbortf);

    bool datOutput::OpenLog(LPCSTR filename)
    {
        return lpdatOutput_OpenLog(filename);
    }

    void datOutput::CloseLog(void)
    {
        lpdatOutput_CloseLog();
    }

    void datOutput::SetOutputMask(UINT mask)
    {
        lpdatOutput_SetOutputMask(mask);
    }

#pragma region "Stream implementation"
    void Stream::DumpOpenFiles(void)
    {
        lpStream_DumpOpenFiles();
    }

    Stream * Stream::Open(LPCSTR filename, bool p1)
    {
        return lpStream_Open(filename, p1);
    }

    Stream * Stream::Create(LPCSTR filename)
    {
        return lpStream_Create(filename);
    }

    int Stream::Read(THIS_ LPVOID dstBuf, int size)
    {
        return lpStream_Read(this, dstBuf, size);
    }

    int Stream::Write(THIS_ const LPVOID srcBuf, int size)
    {
        return lpStream_Write(this, srcBuf, size);
    }

    int Stream::GetCh(THIS_ void)
    {
        return lpStream_GetCh(this);
    }

    int Stream::PutCh(THIS_ unsigned char ch)
    {
        return lpStream_PutCh(this, ch);
    }

    int Stream::Seek(THIS_ int offset)
    {
        return lpStream_Seek(this, offset);
    }

    int Stream::Tell(THIS_ void)
    {
        return lpStream_Tell(this);
    }
#pragma endregion
}