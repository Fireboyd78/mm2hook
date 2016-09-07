#include "mm2.h"

using namespace MM2;

#define MAX_HOOK_PTRS 4096

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

MM2FnHook<void>     lpPrintf                                ( NULL, NULL, 0x4C9720 );
MM2FnHook<void>     lpMessagef                              ( NULL, NULL, 0x4C9750 );
MM2FnHook<void>     lpDisplayf                              ( NULL, NULL, 0x4C9780 );
MM2FnHook<void>     lpWarningf                              ( NULL, NULL, 0x4C97B0 );
MM2FnHook<void>     lpErrorf                                ( NULL, NULL, 0x4C97E0 );
MM2FnHook<void>     lpQuitf                                 ( NULL, NULL, 0x4C9810 );
MM2FnHook<void>     lpAbortf                                ( NULL, NULL, 0x4C9850 );

MM2FnHook<char *>   lpAngelReadString                       ( NULL, NULL, 0x534790 );

MM2FnHook<void>     lpaiMap_Dump                            ( NULL, NULL, 0x538840 );
MM2PtrHook<aiMap>   lpAIMAP_ptr                             ( NULL, NULL, 0x6B2E10 );

MM2FnHook<bool>     lpdatArgParser_Get                      ( NULL, NULL, 0x4C6190 );
MM2FnHook<bool>     lpdatArgParser_Get_1                    ( NULL, NULL, 0x4C61C0 );
MM2FnHook<bool>     lpdatArgParser_Get_2                    ( NULL, NULL, 0x4C6210 );
MM2FnHook<bool>     lpdatArgParser_Get_3                    ( NULL, NULL, 0x4C6260 );

MM2FnHook<void>     lpdatAssetManager_FullPath              ( NULL, NULL, 0x4C55E0 );
MM2FnHook<void>     lpdatAssetManager_FullPath_             ( NULL, NULL, 0x4C56F0 );
MM2FnHook<bool>     lpdatAssetManager_Exists                ( NULL, NULL, 0x4C59B0 );
MM2FnHook<bool>     lpdatAssetManager_Exists_               ( NULL, NULL, 0x4C59E0 );

MM2FnHook<void>     lpdatOutput_CloseLog                    ( NULL, NULL, 0x4C9530 );
MM2FnHook<bool>     lpdatOutput_OpenLog                     ( NULL, NULL, 0x4C9590 );
MM2FnHook<void>     lpdatOutput_SetOutputMask               ( NULL, NULL, 0x4C95A0 );

MM2FnHook<void>     lpdatTimeManager_Reset                  ( NULL, NULL, 0x4C6300 );
MM2FnHook<void>     lpdatTimeManager_Update                 ( NULL, NULL, 0x4C6340 );

MM2FnHook<void>     lpmmHUD_SetMessage                      ( NULL, NULL, 0x42E1F0 );
MM2FnHook<void>     lpmmHUD_SetMessage2                     ( NULL, NULL, 0x42E240 );
MM2FnHook<void>     lpmmHUD_PostChatMessage                 ( NULL, NULL, 0x42D280 );

MM2PtrHook<mmGameManager *>
                    lpmmGameManager_Instance                ( NULL, NULL, 0x5E0D08 );

MM2FnHook<bool>     lpmmGameMusicData_LoadAmbientSFX        ( NULL, NULL, 0x434060 );

MM2FnHook<int>      lpmmPopup_IsEnabled                     ( NULL, NULL, 0x42A280 );
MM2FnHook<void>     lpmmPopup_Lock                          ( NULL, NULL, 0x42B4F0 );
MM2FnHook<void>     lpmmPopup_Unlock                        ( NULL, NULL, 0x42B500 );
MM2FnHook<void>     lpmmPopup_ProcessChat                   ( NULL, NULL, 0x42A400 );

#ifdef IO_EVENT_HOOK
MM2FnHook<bool>     lpioEventQueue_Pop                      ( NULL, NULL, 0x4BA930 );
MM2FnHook<bool>     lpioEventQueue_Peek                     ( NULL, NULL, 0x4BA980 );
MM2FnHook<void>     lpioEventQueue_Queue                    ( NULL, NULL, 0x4BA9D0 );
MM2FnHook<void>     lpioEventQueue_Command                  ( NULL, NULL, 0x4BAA50 );
#endif

MM2FnHook<void>     lpvehCarAudioCtr_SetSirenCSVName        ( NULL, NULL, 0x4D0C80 );

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

    DEFINE_PRINT_HOOK(Printf, lpPrintf);
    DEFINE_PRINT_HOOK(Messagef, lpMessagef);
    DEFINE_PRINT_HOOK(Displayf, lpDisplayf);
    DEFINE_PRINT_HOOK(Warningf, lpWarningf);
    DEFINE_PRINT_HOOK(Errorf, lpErrorf);
    DEFINE_PRINT_HOOK(Quitf, lpQuitf);
    DEFINE_PRINT_HOOK(Abortf, lpAbortf);

    char * AngelReadString(UINT stringId) {
        return lpAngelReadString(stringId);
    }
}

void aiMap::Dump(void) {
    const aiMap* This = lpAIMAP_ptr;
    lpaiMap_Dump(&*This);
}

bool datArgParser::Get(LPCSTR arg) {
    return lpdatArgParser_Get(arg);
}
bool datArgParser::Get(LPCSTR arg, UINT index, int * out) {
    return lpdatArgParser_Get_1(arg, index, out);
}
bool datArgParser::Get(LPCSTR arg, UINT index, float * out) {
    return lpdatArgParser_Get_2(arg, index, out);
}
bool datArgParser::Get(LPCSTR arg, UINT index, LPCSTR * out) {
    return lpdatArgParser_Get_3(arg, index, out);
}

void datAssetManager::FullPath(char * buffer, int length, LPCSTR directory, LPCSTR filename) {
    lpdatAssetManager_FullPath(buffer, length, directory, filename);
}

void datAssetManager::FullPath(char * buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
    lpdatAssetManager_FullPath_(buffer, length, directory, filename, extension);
}

bool MM2::datAssetManager::Exists(LPCSTR directory, LPCSTR filename) {
    return lpdatAssetManager_Exists(directory, filename);
}

bool MM2::datAssetManager::Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
    return lpdatAssetManager_Exists_(directory, filename, extension);
}

bool datOutput::OpenLog(LPCSTR filename) {
    return lpdatOutput_OpenLog(filename);
}

void datOutput::CloseLog(void) {
    lpdatOutput_CloseLog();
}

void datOutput::SetOutputMask(UINT mask) {
    lpdatOutput_SetOutputMask(mask);
}

void datTimeManager::Reset(void) {
    lpdatTimeManager_Reset();

}
void datTimeManager::Update(void) {
    lpdatTimeManager_Update();
}

# ifdef IO_EVENT_HOOK
bool ioEventQueue::Pop(ioEvent *outEvent) {
    return lpioEventQueue_Pop(outEvent);
}

bool ioEventQueue::Peek(ioEvent *outEvent, int *idx) {
    return lpioEventQueue_Peek(outEvent, idx);
}

void ioEventQueue::Queue(ioEvent::ioEventType type, int x, int y, int value) {
    lpioEventQueue_Queue(type, x, y, value);
}

void ioEventQueue::Command(void *command) {
    lpioEventQueue_Command(command);
}
# endif

MM2PtrHook<mmGameManager*> mmGameManager::Instance(void) {
    return lpmmGameManager_Instance;
}

bool mmGameMusicData::LoadAmbientSFX(LPCSTR name) {
    return lpmmGameMusicData_LoadAmbientSFX(this, name);
}

void mmHUD::SetMessage(THIS_ LPCSTR message, float duration, int p2) {
    lpmmHUD_SetMessage(this, message, duration, p2);
}

void mmHUD::SetMessage2(THIS_ LPCSTR message) {
    lpmmHUD_SetMessage2(this, message);
}

void mmHUD::PostChatMessage(THIS_ LPCSTR message) {
    lpmmHUD_PostChatMessage(this, message);
}

int mmPopup::IsEnabled(THIS_ void) {
    return lpmmPopup_IsEnabled(this);
}

void mmPopup::Lock(THIS_ void) {
    lpmmPopup_Lock(this);
}

void mmPopup::Unlock(THIS_ void) {
    lpmmPopup_Unlock(this);
}

void mmPopup::ProcessChat(THIS_ void) {
    lpmmPopup_ProcessChat(this);
}

void vehCarAudioContainer::SetSirenCSVName(LPCSTR name) {
    lpvehCarAudioCtr_SetSirenCSVName(name);
}