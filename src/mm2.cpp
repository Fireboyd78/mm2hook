#include "mm2.h"

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

FnHook<void> lpPrintf;
FnHook<void> lpMessagef;
FnHook<void> lpDisplayf;
FnHook<void> lpWarningf;
FnHook<void> lpErrorf;
FnHook<void> lpQuitf;
FnHook<void> lpAbortf;

static const MM2InitData _funcs_init[] = {
    // IMPORTANT: New versions must be reflected here!

    // FUNCTION     ADDRESS [BETA 1/BETA 2/RETAIL]
    &lpPrintf,      { NULL, NULL, 0x4C9720 },
    &lpMessagef,    { NULL, NULL, 0x4C9750 },
    &lpDisplayf,    { NULL, NULL, 0x4C9780 },
    &lpWarningf,    { NULL, NULL, 0x4C97B0 },
    &lpErrorf,      { NULL, NULL, 0x4C97E0 },
    &lpQuitf,       { NULL, NULL, 0x4C9810 },
    &lpAbortf,      { NULL, NULL, 0x4C9850 },

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

DEFINE_PRINT_HOOK(MM2::Printf, lpPrintf);
DEFINE_PRINT_HOOK(MM2::Messagef, lpMessagef);
DEFINE_PRINT_HOOK(MM2::Displayf, lpDisplayf);
DEFINE_PRINT_HOOK(MM2::Warningf, lpWarningf);
DEFINE_PRINT_HOOK(MM2::Errorf, lpErrorf);
DEFINE_PRINT_HOOK(MM2::Quitf, lpQuitf);
DEFINE_PRINT_HOOK(MM2::Abortf, lpAbortf);

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