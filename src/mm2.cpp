#include "mm2.h"

using namespace MM2;

#define MAX_HOOK_PTRS 4096

#define DEFINE_PRINT_HOOK(x) \
NAKED void x(LPCSTR str, ...) { \
    __asm jmp dword ptr ds:$##x \
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

static MM2Version g_game_version = MM2_INVALID; // current game version

static int g_hook_idx = 0; // next index to use
// global hook pool -- used to directly initialize pointers
static LPVOID g_hook_ptrs[MAX_HOOK_PTRS];

// sets the version to the proper game version
static bool reset_hook(IMM2HookPtr *hook) {
    if (g_game_version != MM2_INVALID) {
        hook->set_version(g_game_version);
        return true;
    }
    return false;
};

static void alloc_hook(IMM2HookPtr *hook) {
    if (g_hook_idx < MAX_HOOK_PTRS) {
        reset_hook(hook); // try to update pointer
        g_hook_ptrs[g_hook_idx++] = hook;
    } else {
        MessageBox(NULL, "FATAL ERROR: No more hook pointers available!", "MM2Hook", MB_OK | MB_ICONERROR);
    }
};

static void dealloc_hook(int hook_idx) {
    if (hook_idx > 0) g_hook_ptrs[hook_idx] = NULL;
};

NOTHROW inline IMM2HookPtr::IMM2HookPtr(const MM2AddressData &addressData) {
    this->addressData = addressData;
    alloc_hook(this);
}

NOTHROW inline IMM2HookPtr::~IMM2HookPtr() {
    dealloc_hook(hook_idx);
}

MM2FnHook<void> $Printf                 ( NULL, NULL, 0x4C9720 );
MM2FnHook<void> $Messagef               ( NULL, NULL, 0x4C9750 );
MM2FnHook<void> $Displayf               ( NULL, NULL, 0x4C9780 );
MM2FnHook<void> $Warningf               ( NULL, NULL, 0x4C97B0 );
MM2FnHook<void> $Errorf                 ( NULL, NULL, 0x4C97E0 );
MM2FnHook<void> $Quitf                  ( NULL, NULL, 0x4C9810 );
MM2FnHook<void> $Abortf                 ( NULL, NULL, 0x4C9850 );

MM2FnHook<char *>   $AngelReadString    ( NULL, NULL, 0x534790 );

CMidtownMadness2::CMidtownMadness2(int engineVersion)
    : CAGEGame(engineVersion) {
    this->m_gameVersion = EngineVersionToGameVersion(engineVersion);

    // game version for hook system to use
    g_game_version = m_gameVersion;
    
    // try to initialize hooks
    IMM2HookPtr *hook;

    int i = 0;
    int numHooks = 0;

    LogFile::WriteLine("Initializing MM2 hooks...");

    while ((hook = (IMM2HookPtr*)g_hook_ptrs[i++]) != NULL) {
        if (reset_hook(hook))
            ++numHooks;
    }

    LogFile::Format(" - numHooks: %d\n", numHooks);

    if (numHooks == 0) {
        LogFile::WriteLine("WARNING: No supported hooks were found for this game version. Crashes may occur!");
    }
}

CMidtownMadness2::CMidtownMadness2(LPAGEGameInfo gameEntry)
    : CMidtownMadness2(gameEntry->age_version) {}

namespace MM2
{
    MM2PtrHook<char>  szCityName                            ( NULL, NULL, 0x6B167C );
    MM2PtrHook<char>  szCityName2                           ( NULL, NULL, 0x6B16A4 );

    DEFINE_PRINT_HOOK(Printf);
    DEFINE_PRINT_HOOK(Messagef);
    DEFINE_PRINT_HOOK(Displayf);
    DEFINE_PRINT_HOOK(Warningf);
    DEFINE_PRINT_HOOK(Errorf);
    DEFINE_PRINT_HOOK(Quitf);
    DEFINE_PRINT_HOOK(Abortf);

    char * AngelReadString(UINT stringId) {
        return $AngelReadString(stringId);
    }
}